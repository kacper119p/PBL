#include "MovementComponent.h"
#include "spdlog/spdlog.h"
#include "Engine/Components/Physics/Rigidbody.h"
namespace Engine
{

    void MovementComponent::Update(float deltaTime)
    {
#if !EDITOR
        glm::vec3 position = GetOwner()->GetTransform()->GetPosition();

        InputManager& input = InputManager::GetInstance();

        if (input.IsKeyPressed(GLFW_KEY_W))
        {
            GetOwner()->GetComponent<RigidBody>()->AddForce(GetOwner()->GetTransform()->GetForward() * Speed);
        }
            
        if (input.IsKeyPressed(GLFW_KEY_S))
        {
            GetOwner()->GetComponent<RigidBody>()->AddForce(-GetOwner()->GetTransform()->GetForward() * Speed);
        }
        if (input.IsKeyPressed(GLFW_KEY_A))
        {
            GetOwner()->GetComponent<RigidBody>()->AddForce(-GetOwner()->GetTransform()->GetRight() * Speed);
        }
            
        if (input.IsKeyPressed(GLFW_KEY_D))
        {
            GetOwner()->GetComponent<RigidBody>()->AddForce(GetOwner()->GetTransform()->GetRight() * Speed);
        }

        #endif
    }

    rapidjson::Value MovementComponent::Serialize(rapidjson::Document::AllocatorType & Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        END_COMPONENT_SERIALIZATION
    }

    void MovementComponent::DeserializeValuePass(const rapidjson::Value& Object,
                                                 Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void MovementComponent::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                      Serialization::ReferenceTable& ReferenceMap)
    {
        // Implementacja
    }
}
