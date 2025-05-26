#include "MovementComponent.h"
#include "spdlog/spdlog.h"
#include "Engine/Components/Physics/RigidBody.h"
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

        glm::vec3 forward = transform->GetForward();

        InputManager& input = InputManager::GetInstance();

        bool isLeftForward = false;
        bool isRightForward = false;
        bool isLeftBackward = false;
        bool isRightBackward = false;

        if (input.IsKeyPressed(GLFW_KEY_W) || input.IsGamepadButtonPressed(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER))
        {
            isLeftForward = true;
        }
        if (input.IsKeyPressed(GLFW_KEY_S) || input.IsGamepadButtonPressed(GLFW_GAMEPAD_BUTTON_LEFT_BUMPER))
        {
            isLeftBackward = true;
        }

        if (input.IsKeyPressed(GLFW_KEY_UP) || input.IsGamepadButtonPressed(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER))
        {
            isRightForward = true;
        }
        if (input.IsKeyPressed(GLFW_KEY_DOWN) || input.IsGamepadButtonPressed(GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER))
        {
            isRightBackward = true;
        }

        if (isLeftForward && isRightForward)
        {
            CurrentVelocity = Speed*(1-smooth) + 1 * smooth;
        }
        else if (isLeftBackward && isRightBackward)
        {
            CurrentVelocity = -(Speed * (1 - smooth) + 1 * smooth);
        }
        else if ((isLeftBackward || isRightBackward)&&!(isLeftForward||isRightForward))
        {
            // Rotate slightly
            float angle = (isLeftBackward ? -1.0f : 1.0f) * RotationSpeed * deltaTime;
            glm::quat rotation = transform->GetRotation() * glm::angleAxis(glm::radians(angle), glm::vec3(0, 1, 0));

            // Move slightly forward in the rotated direction
            CurrentVelocity = -(Speed * 0.2 * (1 - smooth) + 1 * smooth);


            transform->SetEulerAngles(glm::degrees(glm::eulerAngles(rotation)));
        }
        else if ((isLeftForward || isRightForward) && !(isLeftBackward || isRightBackward))
        {
            // Rotate slightly
            float angle = (isLeftForward ? 1.0f : -1.0f) * RotationSpeed * deltaTime;
            glm::quat rotation = transform->GetRotation() * glm::angleAxis(glm::radians(angle), glm::vec3(0, 1, 0));

            // Move slightly forward in the rotated direction
            CurrentVelocity = Speed * 0.2 * (1 - smooth) + 1 * smooth;
            

            transform->SetEulerAngles(glm::degrees(glm::eulerAngles(rotation)));
            
        }
        else if ((isLeftForward && isRightBackward) || (isLeftBackward && isRightForward))
        {
            float angle = (isLeftForward ? 1.0f : -1.0f) * RotationSpeed * deltaTime;
            glm::quat rotation = transform->GetRotation() * glm::angleAxis(glm::radians(angle), glm::vec3(0, 1, 0));

            transform->SetEulerAngles(glm::degrees(glm::eulerAngles(rotation)));
        }
        else
        {
            if (abs(CurrentVelocity)>0.0f)
            CurrentVelocity *=smooth;
        }

        if (!transform)
        {
            spdlog::error("MovementComponent: Attempted to set position on a null transform!");
            return;
        }
        transform->SetPosition(position + CurrentVelocity * forward * deltaTime);


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
