#include "BloomPostprocessingEffect.h"

#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"
#include "glad/glad.h"

namespace Engine
{
    BloomPostprocessingEffect::BloomPostprocessingEffect() :
        PrefilterShader(Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/Bloom/Bloom.vert", nullptr,
                                           "./res/shaders/Bloom/BloomFilter.frag"))),
        DownSampleShader(Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/Bloom/Bloom.vert", nullptr,
                                           "./res/shaders/Bloom/DownSample.frag"))),
        UpSampleShader(Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/Bloom/Bloom.vert", nullptr,
                                           "./res/shaders/Bloom/DownSample.frag"))),
        OutputShader(Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/Bloom/Bloom.vert", nullptr,
                                           "./res/shaders/Bloom/BloomOutput.frag")))
    {
        glGenFramebuffers(1, &FrameBuffer);

        glm::vec2 mipSize(1920.0f, 1080.0f);
        glm::ivec2 mipIntSize(1920, 1080);

        for (BloomMip& mip : BloomMips)
        {
            mipSize *= 0.5f;
            mipIntSize /= 2;
            mip.Size = mipSize;
            mip.IntSize = mipIntSize;

            glGenTextures(1, &mip.Texture);
            glBindTexture(GL_TEXTURE_2D, mip.Texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, static_cast<int>(mipSize.x), static_cast<int>(mipSize.y),
                         0, GL_RGB, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glGenTextures(1, &PrefilteredColor);
        glBindTexture(GL_TEXTURE_2D, PrefilteredColor);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, static_cast<int>(1920), static_cast<int>(1080),
                     0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    void BloomPostprocessingEffect::PrefilterBrightSpots(const uint32_t SceneColorTexture)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        PrefilterShader.Use();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, PrefilteredColor, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SceneColorTexture);
        ScreenQuad.Draw();
    }

    void BloomPostprocessingEffect::Render(const uint32_t SceneColorTexture)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
        DownSampleShader.Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PrefilteredColor);

        for (const BloomMip& mip : BloomMips)
        {
            glViewport(0, 0, static_cast<GLsizei>(mip.Size.x), static_cast<GLsizei>(mip.Size.y));
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D, mip.Texture, 0);
            ScreenQuad.Draw();
            glBindTexture(GL_TEXTURE_2D, mip.Texture);
        }


        UpSampleShader.Use();

        glClear(GL_DEPTH_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);

        for (int i = MipCount - 1; i > 0; --i)
        {
            const BloomMip& mip = BloomMips[i];
            const BloomMip& nextMip = BloomMips[i - 1];

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mip.Texture);

            glViewport(0, 0, static_cast<GLsizei>(nextMip.Size.x), static_cast<GLsizei>(nextMip.Size.y));
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D, nextMip.Texture, 0);

            ScreenQuad.Draw();
        }

        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_BLEND);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        OutputShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SceneColorTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, BloomMips[0].Texture);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, 1920, 1080);

        ScreenQuad.Draw();
    }

    BloomPostprocessingEffect::~BloomPostprocessingEffect()
    {
        for (BloomMip& bloomMip : BloomMips)
        {
            glDeleteTextures(1, &bloomMip.Texture);
        }
        glDeleteTextures(1, &PrefilteredColor);
        glDeleteFramebuffers(1, &FrameBuffer);
    }
} // Engine
