#include "spdlog/spdlog.h"
#include "Engine/Components/Physics/RigidBody.h"
#include "Engine/Components/Game/PlayerAnimationManager.h"
#include "Engine/EngineObjects/Player/DefaultPlayer.h"

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

        AudioManager::GetInstance().PlayAudio(DrivingSound);
        AudioManager::GetInstance().SetSoundPosition(DrivingSound, GetOwner()->GetTransform()->GetPosition());
        AudioManager::GetInstance().SetSoundPosition(BloodCleaningSound, GetOwner()->GetTransform()->GetPosition());

        glm::vec3 position = transform->GetPosition();

        const glm::vec3& eulerAngles = transform->GetEulerAngles();
        float yaw = eulerAngles.y;

        glm::vec3 forward = transform->GetForward();

        InputManager& input = InputManager::GetInstance();
        PlayerAnimationManager* playerAnimationManager = PlayerAnimationManager::GetInstance();

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

        if (DefaultPlayer::GetInstance().GetCurrentTool() != Tool::Broom)
            AudioManager::GetInstance().PauseSound(BloodCleaningSound);

        if (isLeftForward && isRightForward)
        {
            if (DefaultPlayer::GetInstance().GetCurrentTool() == Tool::Broom)
                AudioManager::GetInstance().PlayAudio(BloodCleaningSound);
            rigidbody->AddForce(forward * Speed * 1.25f, ForceMode::Force);
            if (playerAnimationManager)
            {
                playerAnimationManager->TrackLeftForward();
                if (playerAnimationManager->LeftHandPosition != 2 && CanChangeLeftHand())
                {
                    playerAnimationManager->ChangeLeftHandPosition(2);
                    LeftHandChangeTime = glfwGetTime();
                }
                playerAnimationManager->TrackRightForward();
                if (playerAnimationManager->RightHandPosition != 2 && CanChangeRightHand())
                {
                    playerAnimationManager->ChangeRightHandPosition(2);
                    RightHandChangeTime = glfwGetTime();
                }
            }
            AudioManager::GetInstance().SetPitch(DrivingSound, 1.2f);
        }
        else if (isLeftBackward && isRightBackward)
        {
            if (DefaultPlayer::GetInstance().GetCurrentTool() == Tool::Broom)
                AudioManager::GetInstance().PlayAudio(BloodCleaningSound);
            rigidbody->AddForce(-forward * Speed, ForceMode::Force);
            if (playerAnimationManager)
            {
                playerAnimationManager->TrackLeftBackward();
                if (playerAnimationManager->LeftHandPosition != 0 && CanChangeLeftHand())
                {
                    playerAnimationManager->ChangeLeftHandPosition(0);
                    LeftHandChangeTime = glfwGetTime();
                }
                playerAnimationManager->TrackRightBackward();
                if (playerAnimationManager->RightHandPosition != 0 && CanChangeRightHand())
                {
                    playerAnimationManager->ChangeRightHandPosition(0);
                    RightHandChangeTime = glfwGetTime();
                }
            }
            AudioManager::GetInstance().SetPitch(DrivingSound, 1.0f);
        }
        else if ((isLeftForward && isRightBackward) || (isLeftBackward && isRightForward))
        {
            if (DefaultPlayer::GetInstance().GetCurrentTool() == Tool::Broom)
                AudioManager::GetInstance().PlayAudio(BloodCleaningSound);
            rigidbody->AddTorque(glm::vec3(0, (isLeftForward ? -1.0f : 1.0f) * BothRotationSpeed, 0), ForceMode::Force);
            if (isLeftForward)
            {
                if (playerAnimationManager)
                {
                    playerAnimationManager->TrackLeftForward();
                    if (playerAnimationManager->LeftHandPosition != 2 && CanChangeLeftHand())
                    {
                        playerAnimationManager->ChangeLeftHandPosition(2);
                        LeftHandChangeTime = glfwGetTime();
                    }
                    playerAnimationManager->TrackRightBackward();
                    if (playerAnimationManager->RightHandPosition != 0 && CanChangeRightHand())
                    {
                        playerAnimationManager->ChangeRightHandPosition(0);
                        RightHandChangeTime = glfwGetTime();
                    }
                }
            }
            else
            {
                if (playerAnimationManager)
                {
                    playerAnimationManager->TrackLeftBackward();
                    if (playerAnimationManager->LeftHandPosition != 0 && CanChangeLeftHand())
                    {
                        playerAnimationManager->ChangeLeftHandPosition(0);
                        LeftHandChangeTime = glfwGetTime();
                    }
                    playerAnimationManager->TrackRightForward();
                    if (playerAnimationManager->RightHandPosition != 2 && CanChangeRightHand())
                    {
                        playerAnimationManager->ChangeRightHandPosition(2);
                        RightHandChangeTime = glfwGetTime();
                    }
                }
            }
            AudioManager::GetInstance().SetPitch(DrivingSound, 1.2f);
        }
        else if ((isLeftBackward || isRightBackward) && !(isLeftForward || isRightForward))
        {
            if (DefaultPlayer::GetInstance().GetCurrentTool() == Tool::Broom)
                AudioManager::GetInstance().PlayAudio(BloodCleaningSound);
            rigidbody->AddTorque(glm::vec3(0, (isLeftBackward ? 1.0f : -1.0f) * RotationSpeed, 0), ForceMode::Force);
            rigidbody->AddForce(-forward * Speed, ForceMode::Force);
            if (playerAnimationManager)
            {
                if (isLeftBackward)
                {
                    playerAnimationManager->TrackLeftBackward();
                    if (playerAnimationManager->LeftHandPosition != 0 && CanChangeLeftHand())
                    {
                        playerAnimationManager->ChangeLeftHandPosition(0);
                        LeftHandChangeTime = glfwGetTime();
                    }
                    playerAnimationManager->TrackRightStop();
                    if (playerAnimationManager->RightHandPosition != 1 && CanChangeRightHand())
                    {
                        playerAnimationManager->ChangeRightHandPosition(1);
                        RightHandChangeTime = glfwGetTime();
                    }
                }
                else
                {
                    playerAnimationManager->TrackRightBackward();
                    if (playerAnimationManager->RightHandPosition != 0 && CanChangeRightHand())
                    {
                        playerAnimationManager->ChangeRightHandPosition(0);
                        RightHandChangeTime = glfwGetTime();
                    }
                    playerAnimationManager->TrackLeftStop();
                    if (playerAnimationManager->LeftHandPosition != 1 && CanChangeLeftHand())
                    {
                        playerAnimationManager->ChangeLeftHandPosition(1);
                        LeftHandChangeTime = glfwGetTime();
                    }
                }
            }
            AudioManager::GetInstance().SetPitch(DrivingSound, 1.0f);
        }
        else if ((isLeftForward || isRightForward) && !(isLeftBackward || isRightBackward))
        {
            if (DefaultPlayer::GetInstance().GetCurrentTool() == Tool::Broom)
                AudioManager::GetInstance().PlayAudio(BloodCleaningSound);
            rigidbody->AddTorque(glm::vec3(0, (isLeftForward ? -1.0f : 1.0f) * RotationSpeed, 0), ForceMode::Force);
            rigidbody->AddForce(forward * Speed, ForceMode::Force);
            if (playerAnimationManager)
            {
                if (isLeftForward)
                {
                    playerAnimationManager->TrackLeftForward();
                    if (playerAnimationManager->LeftHandPosition != 2 && CanChangeLeftHand())
                    {
                        playerAnimationManager->ChangeLeftHandPosition(2);
                        LeftHandChangeTime = glfwGetTime();
                    }
                    playerAnimationManager->TrackRightStop();
                    if (playerAnimationManager->RightHandPosition != 1 && CanChangeRightHand())
                    {
                        playerAnimationManager->ChangeRightHandPosition(1);
                        RightHandChangeTime = glfwGetTime();
                    }
                }
                else
                {
                    playerAnimationManager->TrackRightForward();
                    if (playerAnimationManager->RightHandPosition != 2 && CanChangeRightHand())
                    {
                        playerAnimationManager->ChangeRightHandPosition(2);
                        RightHandChangeTime = glfwGetTime();
                    }
                    playerAnimationManager->TrackLeftStop();
                    if (playerAnimationManager->LeftHandPosition != 1 && CanChangeLeftHand())
                    {
                        playerAnimationManager->ChangeLeftHandPosition(1);
                        LeftHandChangeTime = glfwGetTime();
                    }
                }
            }
            AudioManager::GetInstance().SetPitch(DrivingSound, 1.2f);
        }
        else
        {
            if (DefaultPlayer::GetInstance().GetCurrentTool() == Tool::Broom)
                AudioManager::GetInstance().PauseSound(BloodCleaningSound);
            if (playerAnimationManager)
            {
                playerAnimationManager->StopAllAnimations();
                if (playerAnimationManager->LeftHandPosition != 1 && CanChangeLeftHand())
                {
                    playerAnimationManager->ChangeLeftHandPosition(1);
                    LeftHandChangeTime = glfwGetTime();
                }
                if (playerAnimationManager->RightHandPosition != 1 && CanChangeRightHand())
                {
                    playerAnimationManager->ChangeRightHandPosition(1);
                    RightHandChangeTime = glfwGetTime();
                }
            }
            AudioManager::GetInstance().SetPitch(DrivingSound, 0.8f);
        }

        if (!transform)
        {
            spdlog::error("MovementComponent: Attempted to set position on a null transform!");
            return;
        }


#endif
    }

    bool MovementComponent::CanChangeLeftHand()
    {
        return glfwGetTime() - LeftHandChangeTime > 0.2f;
    }

    bool MovementComponent::CanChangeRightHand()
    {
        return glfwGetTime() - RightHandChangeTime > 0.2f;
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
