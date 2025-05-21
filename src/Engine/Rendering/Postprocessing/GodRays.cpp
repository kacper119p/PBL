#include "GodRays.h"

#include "Engine/EngineObjects/LightManager.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

namespace Engine
{
    GodRays::GodRays() :
        Resolution(1920 / 4, 1080 / 4),
        BlendShader(Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/GodRays/GodRays.vert", nullptr,
                                           "./res/shaders/GodRays/GodRays.frag"))),
        Shader(Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/GodRays/GodRays.vert", nullptr,
                                           "./res/shaders/GodRays/Blend.frag")))
    {
        glGenFramebuffers(1, &FrameBuffer);

        glGenTextures(1, &OutputTexture);
        glBindTexture(GL_TEXTURE_2D, OutputTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, Resolution.x, Resolution.y, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, OutputTexture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    GodRays::~GodRays()
    {
        glDeleteTextures(1, &OutputTexture);
        glDeleteFramebuffers(1, &FrameBuffer);
    }

    void GodRays::Render(const SceneFrameBuffer& SceneFrameBuffer) const
    {
        Shader.Use();
        LightManager::GetInstance()->BindLightScreenPositionBuffer();
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
        glViewport(0, 0, Resolution.x, Resolution.y);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SceneFrameBuffer.GetResolvedOcclusionBuffer());
        ScreenQuad.Draw();

        SceneFrameBuffer.BindResolved();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, OutputTexture);
        glViewport(0, 0, 1920, 1080);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);
        BlendShader.Use();
        ScreenQuad.Draw();
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_BLEND);
    }
} // Engine
