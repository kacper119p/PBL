#include "BloodEraser.h"

#include "BloodManager.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/Rendering/Plane.h"
#include "Engine/Textures/TextureManager.h"
#include "Serialization/SerializationUtility.h"
#include "Shaders/Shader.h"
#include "glad/glad.h"

#if EDITOR
#include "Materials/Material.h"
#endif

namespace Engine
{
    BloodEraser::BloodEraser()
    {
        Texture = TextureManager::GetTexture("res/textures/BloodSplatter/BloodTest.dds");
#if EDITOR
        SetMaterial(Materials::MaterialManager::GetMaterial("res/materials/Editor/BloodDebug.mat"));
#endif
    }

    BloodEraser::~BloodEraser()
    {
        BloodManager::GetCurrent()->RemoveBloodEraser(this);
    }

    void BloodEraser::Start()
    {
#if EDITOR
        Renderer::Start();
#endif
        if (BloodManager* bloodManager = BloodManager::GetCurrent())
        {
            bloodManager->AddBloodEraser(this);
        }
    }

    void BloodEraser::Draw() const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture.GetId());
        Shaders::Shader::SetUniform(BloodManager::GetCurrent()->GetModelMatrixUniformLocation(),
                                    GetOwner()->GetTransform()->GetLocalToWorldMatrix());
        Shaders::Shader::SetUniform(BloodManager::GetCurrent()->GetColorUniformLocation(), glm::vec3(0.0f));
        Plane::Draw();
    }

#if EDITOR
    void BloodEraser::DrawImGui()
    {
        if (ImGui::CollapsingHeader("Blood Eraser"))
        {
        }
    }

    void BloodEraser::RenderDepth(const CameraRenderData& RenderData)
    {
    }

    void BloodEraser::Render(const CameraRenderData& RenderData)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture.GetId());
        const Shaders::Shader shader = Material->GetMainPass();
        shader.SetUniform("ViewMatrix", RenderData.ViewMatrix);
        shader.SetUniform("ProjectionMatrix", RenderData.ProjectionMatrix);
        shader.SetUniform("ObjectToWorldMatrix",
                          GetOwner()->GetTransform()->GetLocalToWorldMatrix());
        shader.SetUniform("Color", glm::vec3(1.0f, 0.0f, 1.0f));
        Plane::Draw();
    }

    void BloodEraser::RenderDirectionalShadows(const CameraRenderData& RenderData)
    {
    }

    void BloodEraser::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                             const glm::mat4* SpaceTransformMatrices)
    {
    }
#endif

    rapidjson::Value BloodEraser::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(Texture)
        END_COMPONENT_SERIALIZATION
    }

    void BloodEraser::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(Texture)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void BloodEraser::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
}
