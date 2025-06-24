#include "TransitionTint.h"

#include <glad/glad.h>

#include "Engine/Rendering/ScreenQuad.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

namespace Engine
{
    Shaders::Shader TransitionTint::BlendShader = Shaders::Shader();
    int32_t TransitionTint::TintUniformLocation = -1;
    bool TransitionTint::Enabled = false;

    void TransitionTint::Initialize()
    {
        BlendShader = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/TransitionTint/TransitionTint.vert",
                                           "",
                                           "./res/shaders/TransitionTint/TransitionTint.frag"));
        TintUniformLocation = BlendShader.GetUniformLocation("-Tint");
    }

    void TransitionTint::Play()
    {
    }

    void TransitionTint::Render()
    {
        if (!Enabled)
        {
            return;
        }
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        BlendShader.Use();
        Shaders::Shader::SetUniform(TintUniformLocation, glm::vec3(0.5));
        Rendering::ScreenQuad::Draw();
        glDisable(GL_BLEND);
    }
}
