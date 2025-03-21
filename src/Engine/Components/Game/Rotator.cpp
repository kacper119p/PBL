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
        rapidjson::Value object(rapidjson::kObjectType);
        object.AddMember("velocity", Serialization::Serialize(Velocity, Allocator), Allocator);
        return object;
    }

    void Rotator::DeserializeValuePass(const rapidjson::Value& Object)
    {
    }

    void Rotator::DeserializeReferencesPass(const rapidjson::Value& Object,
                                            std::unordered_map<GUID, SerializedObject*> ReferenceMap)
    {
    }
} // Engine
