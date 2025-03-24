#include "SpotLight.h"
#include "Engine/EngineObjects/LightManager.h"
#include "Engine/Gui/LightsGui.h"
#include "Serialization/SerializationUtility.h"

namespace Engine
{
    void SpotLight::OnAdd(Entity* NewOwner)
    {
        Component::OnAdd(NewOwner);
        LightManager::GetInstance()->RegisterLight(this);
        LightsGui::RegisterLight(this);
    }

    SpotLight::~SpotLight()
    {
        LightManager::GetInstance()->UnregisterLight(this);
        LightsGui::UnregisterLight(this);
    }

    rapidjson::Value SpotLight::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(Color)
        SERIALIZE_FIELD(OuterAngle)
        SERIALIZE_FIELD(InnerAngle)
        SERIALIZE_FIELD(LinearFalloff)
        SERIALIZE_FIELD(QuadraticFalloff)
        SERIALIZE_FIELD(Range)
        END_COMPONENT_SERIALIZATION
    }

    void SpotLight::DeserializeValuePass(const rapidjson::Value& Object,
                                         Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(Color)
        DESERIALIZE_VALUE(OuterAngle)
        DESERIALIZE_VALUE(InnerAngle)
        DESERIALIZE_VALUE(LinearFalloff)
        DESERIALIZE_VALUE(QuadraticFalloff)
        DESERIALIZE_VALUE(Range)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void SpotLight::DeserializeReferencesPass(const rapidjson::Value& Object,
                                              Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
} // Engine
