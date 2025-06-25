#include "GodRays.h"

#include "Engine/EngineObjects/LightManager.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

namespace Engine
{
    GodRays::GodRays(const glm::uvec2 Resolution) :
        Resolution(Resolution.x, Resolution.y),
        Shader(Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/GodRays/GodRays.vert", nullptr,
                                           "./res/shaders/GodRays/Blend.frag"))),
        BlendShader(Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/GodRays/GodRays.vert", nullptr,
                                           "./res/shaders/GodRays/GodRays.frag")))
    {
        glGenFramebuffers(1, &FrameBuffer);

        glGenTextures(1, &OutputTexture);
        glBindTexture(GL_TEXTURE_2D, OutputTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F,
                     static_cast<GLsizei>(Resolution.x / 2), static_cast<GLsizei>(Resolution.y / 2),
                     0, GL_RGB, GL_FLOAT, nullptr);
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
        glViewport(0, 0, Resolution.x / 2, Resolution.y / 2);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SceneFrameBuffer.GetResolvedOcclusionBuffer());
        Rendering::ScreenQuad::Draw();

        SceneFrameBuffer.BindResolved();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, OutputTexture);
        glViewport(0, 0, Resolution.x, Resolution.y);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);
        BlendShader.Use();
        Rendering::ScreenQuad::Draw();
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_BLEND);
    }
} // Engine
