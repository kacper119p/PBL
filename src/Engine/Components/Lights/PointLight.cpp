#include "PointLight.h"
#include "Engine/EngineObjects/LightManager.h"
#include "Engine/Gui/LightsGui.h"
#include "Serialization/SerializationUtility.h"

namespace Engine
{
    void PointLight::Start()
    {
        Component::Start();
        LightManager::GetInstance()->RegisterLight(this);
        LightsGui::RegisterLight(this);
    }

    PointLight::~PointLight()
    {
        LightManager::GetInstance()->UnregisterLight(this);
        LightsGui::UnregisterLight(this);
    }

    rapidjson::Value PointLight::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(Color)
        SERIALIZE_FIELD(LinearFalloff)
        SERIALIZE_FIELD(QuadraticFalloff)
        SERIALIZE_FIELD(Range)
        END_COMPONENT_SERIALIZATION
    }

    void PointLight::DeserializeValuePass(const rapidjson::Value& Object,
                                          Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(Color)
        DESERIALIZE_VALUE(LinearFalloff)
        DESERIALIZE_VALUE(QuadraticFalloff)
        DESERIALIZE_VALUE(Range)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void PointLight::DeserializeReferencesPass(const rapidjson::Value& Object,
                                               Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
} // Engine
