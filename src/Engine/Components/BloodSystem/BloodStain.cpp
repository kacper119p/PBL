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
        BloodSourceBase::Start();
        if (BloodManager* bloodManager = BloodManager::GetCurrent())
        {
            bloodManager->AddBloodStain(this);
        }
    }

#if EDITOR
    void BloodStain::DrawImGui()
    {
        if (ImGui::CollapsingHeader("Blood Stain"))
        {
            BloodSourceBase::DrawImGui();
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
