#include "MovementComponent.h"
#include "spdlog/spdlog.h"
namespace Engine
{

    void MovementComponent::Update(float deltaTime)
    {
#if !EDITOR
        Transform* transform = GetOwner() ? GetOwner()->GetTransform() : nullptr;
        if (!transform)
        {
            spdlog::error("MovementComponent: Transform is null!");
            return;
        }

        glm::vec3 position = transform->GetPosition();

        const glm::vec3& eulerAngles = transform->GetEulerAngles();
        float yaw = eulerAngles.y;

        glm::vec3 forward = glm::vec3(sin(glm::radians(yaw)), 0.0f, -cos(glm::radians(yaw)));

        InputManager& input = InputManager::GetInstance();

        bool isLeftForward = false;
        bool isRightForward = false;

        if (input.IsKeyPressed(GLFW_KEY_W) || input.IsGamepadButtonPressed(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER))
        {
            isLeftForward = true;
        }

        if (input.IsKeyPressed(GLFW_KEY_UP) || input.IsGamepadButtonPressed(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER))
        {
            isRightForward = true;
        }

        float rotationSpeed = 45.0f; // degrees per second
        float moveSpeed = Speed; // base movement speed

        if (isLeftForward && isRightForward)
        {
            position += forward * moveSpeed * deltaTime;
        }
        else if (isLeftForward || isRightForward)
        {
            // Rotate slightly
            float angle = (isLeftForward ? -1.0f : 1.0f) * rotationSpeed * deltaTime;
            glm::quat rotation = glm::angleAxis(glm::radians(angle), glm::vec3(0, 1, 0));
            glm::vec3 newForward = rotation * forward;

            // Move slightly forward in the rotated direction
            position += newForward * (moveSpeed * 0.5f) * deltaTime;

            // Apply rotation to the transform
            transform->SetEulerAngles(rotation * transform->GetEulerAngles());
        }

        if (!transform)
        {
            spdlog::error("MovementComponent: Attempted to set position on a null transform!");
            return;
        }
        transform->SetPosition(position);


#endif
    }

    rapidjson::Value MovementComponent::Serialize(rapidjson::Document::AllocatorType& Allocator) const
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
} // namespace Engine
