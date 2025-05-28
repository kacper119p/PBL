#include "BloodSourceBase.h"

#include "BloodManager.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/Rendering/Plane.h"
#include "Materials/Material.h"
#include "Shaders/Shader.h"

#if EDITOR
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#endif

namespace Engine
{
    BloodSourceBase::BloodSourceBase()
    {
        Texture = TextureManager::GetTexture("res/textures/BloodSplatter/BloodTest.dds");
    }

    void BloodSourceBase::Start()
    {
#if EDITOR
        Renderer::Start();
#endif
    }

    void BloodSourceBase::Draw() const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture.GetId());
        Shaders::Shader::SetUniform(BloodManager::GetCurrent()->GetModelMatrixUniformLocation(),
                                    GetOwner()->GetTransform()->GetLocalToWorldMatrix());
        Shaders::Shader::SetUniform(BloodManager::GetCurrent()->GetColorUniformLocation(), Color);
        Plane::Draw();
    }

#if EDITOR
    void BloodSourceBase::DrawImGui()
    {
        ImGui::ColorEdit3("Color##BloodSourceBase", glm::value_ptr(Color));
    }

    void BloodSourceBase::RenderDepth(const CameraRenderData& RenderData)
    {
    }

    void BloodSourceBase::Render(const CameraRenderData& RenderData)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture.GetId());
        const Shaders::Shader shader = Material->GetMainPass();
        shader.SetUniform("ViewMatrix", RenderData.ViewMatrix);
        shader.SetUniform("ProjectionMatrix", RenderData.ProjectionMatrix);
        shader.SetUniform("ObjectToWorldMatrix",
                          GetOwner()->GetTransform()->GetLocalToWorldMatrix());
        shader.SetUniform("Color", Color);
        Plane::Draw();
    }

    void BloodSourceBase::RenderDirectionalShadows(const CameraRenderData& RenderData)
    {
    }

    void BloodSourceBase::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                                 const glm::mat4* SpaceTransformMatrices)
    {
    }
#endif
}
