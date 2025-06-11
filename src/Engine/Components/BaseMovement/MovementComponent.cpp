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

        Rigidbody* rigidbody = GetOwner()->GetComponent<Rigidbody>();

        if (input.IsKeyPressed(GLFW_KEY_W) || input.GetGamepadAxis(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER) > 0.5f)
        {
            isLeftForward = true;
        }
        if (input.IsKeyPressed(GLFW_KEY_S) || input.IsGamepadButtonPressed(GLFW_GAMEPAD_BUTTON_LEFT_BUMPER))
        {
            isLeftBackward = true;
        }

        if (input.IsKeyPressed(GLFW_KEY_UP) || input.GetGamepadAxis(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.5f)
        {
            isRightForward = true;
        }
        if (input.IsKeyPressed(GLFW_KEY_DOWN) || input.IsGamepadButtonPressed(GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER))
        {
            isRightBackward = true;
        }


        if (isLeftForward && isRightForward)
        {
            rigidbody->AddForce(forward * Speed*1.25f, ForceMode::Force);
            
        }
        else if (isLeftBackward && isRightBackward)
        {
            rigidbody->AddForce(-forward * Speed, ForceMode::Force);
        }
        else if ((isLeftForward && isRightBackward) || (isLeftBackward && isRightForward))
        {
            rigidbody->AddTorque(glm::vec3(0, (isLeftForward ? -1.0f : 1.0f)*BothRotationSpeed, 0), ForceMode::Force);
            
        }
        else if ((isLeftBackward || isRightBackward)&&!(isLeftForward||isRightForward))
        {
            rigidbody->AddTorque(glm::vec3(0, (isLeftBackward ? 1.0f : -1.0f)*RotationSpeed, 0), ForceMode::Force);
            rigidbody->AddForce(-forward * Speed, ForceMode::Force);
        }
        else if ((isLeftForward || isRightForward) && !(isLeftBackward || isRightBackward))
        {
            rigidbody->AddTorque(glm::vec3(0, (isLeftForward ? -1.0f : 1.0f)*RotationSpeed, 0), ForceMode::Force);
            rigidbody->AddForce(forward * Speed, ForceMode::Force);

            
        }

        if (!transform)
        {
            spdlog::error("MovementComponent: Attempted to set position on a null transform!");
            return;
        }


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
