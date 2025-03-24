#include "Rotator.h"

#include "Serialization/SerializationUtility.h"

namespace Engine
{
    Rotator::Rotator(const glm::vec3& Velocity) :
        Velocity(Velocity)
    {
    }

    void Rotator::Update(float DeltaTime)
    {
        GetOwner()->GetTransform()->SetEulerAngles(GetOwner()->GetTransform()->GetEulerAngles() + Velocity * DeltaTime);
    }

    rapidjson::Value Rotator::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(Velocity)
        END_COMPONENT_SERIALIZATION
    }

    void Rotator::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(Velocity)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void Rotator::DeserializeReferencesPass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
} // Engine
