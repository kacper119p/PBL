#include "BloodSourceBase.h"

#include "BloodManager.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/Rendering/Plane.h"
#include "Engine/Textures/TextureManager.h"
#include "Shaders/Shader.h"

#if EDITOR
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include "Materials/Material.h"
#include "Helper.h"
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
        ImGui::Separator();
        ImGui::ColorEdit3("Color##BloodSourceBase", glm::value_ptr(Color));
        ImGui::Separator();
        static std::vector<std::string> availableTextures;
        static bool scanned = false;

        Helpers::GetTextureList(availableTextures, scanned);

        static bool showBaseTexPopup = false;

        Helpers::ChangeTexture("BloodSourceBase", Texture, showBaseTexPopup, availableTextures,
                               "BloodSourceBasePicker");
        ImGui::Separator();
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
