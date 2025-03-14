#include "BloomPostprocessingEffect.h"
#include "glad/glad.h"

namespace Engine
{
    BloomPostprocessingEffect::BloomPostprocessingEffect() :
        FilterShader(Shaders::Shader(Shaders::ShaderSourceFiles(
                "./res/shaders/Bloom/Bloom.vert",
                nullptr,
                "./res/shaders/Bloom/BloomFilter.frag"))),
        BlurShaders{
                Shaders::Shader(Shaders::ShaderSourceFiles(
                        "./res/shaders/Bloom/Bloom.vert",
                        nullptr,
                        "./res/shaders/Bloom/BloomHorizontal.frag")),
                Shaders::Shader(Shaders::ShaderSourceFiles(
                        "./res/shaders/Bloom/Bloom.vert",
                        nullptr,
                        "./res/shaders/Bloom/BloomVertical.frag"))
        },
        OutputShader(Shaders::Shader(Shaders::ShaderSourceFiles(
                "./res/shaders/Bloom/Bloom.vert",
                nullptr,
                "./res/shaders/Bloom/BloomOutput.frag")))
    {
        glGenFramebuffers(2, FrameBuffers);
        glGenTextures(2, ColorBuffers);

        for (int i = 0; i < 2; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffers[i]);

            glBindTexture(GL_TEXTURE_2D, ColorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorBuffers[i], 0);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void BloomPostprocessingEffect::Render(unsigned int SceneColorTexture)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffers[1]);

        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        FilterShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SceneColorTexture);
        FilterShader.SetUniform("Threshold", 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        ScreenQuad.Draw();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        constexpr int passCount = 10;

        for (int i = 0; i < passCount; ++i)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffers[i % 2]);
            BlurShaders[i % 2].Use();
            glBindTexture(GL_TEXTURE_2D, ColorBuffers[(i + 1) % 2]);
            glClear(GL_COLOR_BUFFER_BIT);
            ScreenQuad.Draw();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        OutputShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SceneColorTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, ColorBuffers[(passCount - 1) % 2]);

        OutputShader.SetUniform("Exposure", 0.25f);
        OutputShader.SetUniform("Intensity", 0.1f);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        ScreenQuad.Draw();
    }

    BloomPostprocessingEffect::~BloomPostprocessingEffect()
    {
        glDeleteFramebuffers(2, FrameBuffers);
        glDeleteTextures(2, ColorBuffers);
    }
} // Engine
