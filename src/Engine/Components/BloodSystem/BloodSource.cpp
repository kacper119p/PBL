#include "BloodSource.h"

#include "BloodManager.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/Textures/TextureManager.h"
#include "Serialization/SerializationUtility.h"

namespace Engine
{
    BloodSource::BloodSource()
    {
#if EDITOR
        SetMaterial(Materials::MaterialManager::GetMaterial("res/materials/Editor/BloodDebug.mat"));
#endif
    }

    void BloodSource::Draw() const
    {
        BloodSourceBase::Draw();
    }

    void BloodSource::Start()
    {
        BloodSourceBase::Start();
        if (BloodManager* bloodManager = BloodManager::GetCurrent())
        {
            bloodManager->AddBloodSource(this);
        }
    }

#if EDITOR
    void BloodSource::DrawImGui()
    {
        if (ImGui::CollapsingHeader("Blood Stain"))
        {
            BloodSourceBase::DrawImGui();
        }
    }
#endif

    rapidjson::Value BloodSource::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(Texture);
        SERIALIZE_FIELD(Color);
        END_COMPONENT_SERIALIZATION
    }

    void BloodSource::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(Texture)
        DESERIALIZE_VALUE(Color)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void BloodSource::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
}
