#include "BloodStain.h"

#include "BloodManager.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/Rendering/Plane.h"
#include "Engine/Textures/TextureManager.h"
#include "Serialization/SerializationUtility.h"

namespace Engine
{
    BloodStain::BloodStain()
    {
        Texture = TextureManager::GetTexture("res/textures/BloodSplatter/BloodTest.dds");
#if EDITOR
        SetMaterial(Materials::MaterialManager::GetMaterial("res/materials/Editor/BloodDebug.mat"));
#endif
    }

    void BloodStain::Draw() const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture.GetId());
        Shaders::Shader::SetUniform(BloodManager::GetCurrent()->GetModelMatrixUniformLocation(),
                                    GetOwner()->GetTransform()->GetLocalToWorldMatrix());
        Shaders::Shader::SetUniform(BloodManager::GetCurrent()->GetColorUniformLocation(), Color);
        Plane::Draw();
        GetOwner()->Destroy();
    }

    void BloodStain::Start()
    {
#if EDITOR
        Renderer::Start();
#endif
        if (BloodManager* bloodManager = BloodManager::GetCurrent())
        {
            bloodManager->AddBloodStain(this);
        }
    }

#if EDITOR
    void BloodStain::RenderDepth(const CameraRenderData& RenderData)
    {
    }

    void BloodStain::Render(const CameraRenderData& RenderData)
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

    void BloodStain::RenderDirectionalShadows(const CameraRenderData& RenderData)
    {
    }

    void BloodStain::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                            const glm::mat4* SpaceTransformMatrices)
    {
    }

    void BloodStain::DrawImGui()
    {
        if (ImGui::CollapsingHeader("Blood Stain"))
        {
            ImGui::InputFloat3("Color##BloodStain", glm::value_ptr(Color));
        }
    }
#endif

    rapidjson::Value BloodStain::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(Texture);
        SERIALIZE_FIELD(Color);
        END_COMPONENT_SERIALIZATION
    }

    void BloodStain::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(Texture)
        DESERIALIZE_VALUE(Color)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void BloodStain::DeserializeReferencesPass(const rapidjson::Value& Object,
                                               Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
}
