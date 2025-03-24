#include "ShipRoller.h"
#include "Engine/EngineObjects/Entity.h"
#include "GLFW/glfw3.h"
#include "Serialization/SerializationUtility.h"

namespace Engine
{
    ShipRoller::ShipRoller(const glm::vec3& Amplitude, const glm::vec3& Velocity) :
        Amplitude(Amplitude), Velocity(Velocity)
    {
    }

    void ShipRoller::Update(float DeltaTime)
    {
        const float time = static_cast<float>(glfwGetTime());
        GetOwner()->GetTransform()->SetEulerAngles(glm::vec3(
                InitialRotation.x + sin(time * Velocity.x) * Amplitude.x,
                InitialRotation.y + cos(time * Velocity.y) * Amplitude.y,
                InitialRotation.z + sin(time * Velocity.z + 1.57f) * Amplitude.z));
    }

    rapidjson::Value ShipRoller::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(Velocity)
        SERIALIZE_FIELD(Amplitude)
        END_COMPONENT_SERIALIZATION
    }

    void ShipRoller::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(Velocity)
        DESERIALIZE_VALUE(Amplitude)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void ShipRoller::DeserializeReferencesPass(const rapidjson::Value& Object,
                                               Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
} // Engine
