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
        object.AddMember("color", Serialization::Serialize(Color, Allocator), Allocator);
        object.AddMember("outerAngle", Serialization::Serialize(OuterAngle, Allocator), Allocator);
        object.AddMember("innerAngle", Serialization::Serialize(InnerAngle, Allocator), Allocator);
        object.AddMember("linearFalloff", Serialization::Serialize(LinearFalloff, Allocator), Allocator);
        object.AddMember("quadraticFalloff", Serialization::Serialize(QuadraticFalloff, Allocator), Allocator);
        object.AddMember("range", Serialization::Serialize(Range, Allocator), Allocator);
        END_COMPONENT_SERIALIZATION
    }

    void SpotLight::DeserializeValuePass(const rapidjson::Value& Object,
                                         Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        Serialization::Deserialize(Object, "color", Color);
        Serialization::Deserialize(Object, "outerAngle", OuterAngle);
        Serialization::Deserialize(Object, "innerAngle", InnerAngle);
        Serialization::Deserialize(Object, "linearFalloff", LinearFalloff);
        Serialization::Deserialize(Object, "quadraticFalloff", QuadraticFalloff);
        Serialization::Deserialize(Object, "range", Range);
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void SpotLight::DeserializeReferencesPass(const rapidjson::Value& Object,
                                              Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
} // Engine
