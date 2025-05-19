#include "GodRays.h"

#include "Engine/EngineObjects/LightManager.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

namespace Engine
{
    GodRays::GodRays() :
        Resolution(1920 / 8, 1080 / 8),
        Shader(Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/GodRays/GodRays.vert", nullptr,
                                           "./res/shaders/GodRays/GodRays.frag")))
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

    void GodRays::Render(const uint32_t SceneColor, const uint32_t OcclusionMap) const
    {
        Shader.Use();
        LightManager::GetInstance()->BindLightScreenPositionBuffer();
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
        glViewport(0, 0, Resolution.x, Resolution.y);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SceneColor);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, OcclusionMap);

        ScreenQuad.Draw();
    }
} // Engine
