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
        START_OBJECT_SERIALIZATION
        object.AddMember("velocity", Serialization::Serialize(Velocity, Allocator), Allocator);
        END_OBJECT_SERIALIZATION
    }

    void Rotator::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_OBJECT_DESERIALIZATION_VALUE_PASS
        Serialization::Deserialize(Object["velocity"], Velocity);
        END_OBJECT_DESERIALIZATION_VALUE_PASS
    }

    void Rotator::DeserializeReferencesPass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_OBJECT_DESERIALIZATION_REFERENCES_PASS
        END_OBJECT_DESERIALIZATION_REFERENCES_PASS
    }
} // Engine
