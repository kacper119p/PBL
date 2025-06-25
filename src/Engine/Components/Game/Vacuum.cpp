#include "Vacuum.h"
#include "Engine/EngineObjects/Entity.h"
#include "Thrash.h"
#include "Engine/EngineObjects/UpdateManager.h"
#include "Engine/Components/Physics/Rigidbody.h"
#include "Engine/EngineObjects/Scene/Scene.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Components/Colliders/SphereCollider.h"
#include "Engine/Components/Game/ThrashManager.h"
#include "Engine/Components/Game/PlayerAnimationManager.h"
#include <iostream>
#include "Engine/Components/AI/AiManager.h"
#include "Materials/VacuumIndicatorMaterial.h"

namespace Engine
{
    void Vacuum::Start()
    {
        if (this->GetOwner()->GetComponent<Engine::SphereCollider>())
        {
            collider = this->GetOwner()->GetComponent<Engine::SphereCollider>();
        }
        else
        {
            collider = this->GetOwner()->AddComponent<Engine::SphereCollider>();
        }
        collider->SetTrigger(true);
        blowingSound = AudioManager::GetInstance().CreateSoundInstance("strzeelanie odkurzacz");
        suckingSound = AudioManager::GetInstance().CreateSoundInstance("odkurzaczito");
        AudioManager::GetInstance().SetLooping(suckingSound, true);
        Engine::UpdateManager::GetInstance()->RegisterComponent(this);
    }

    void Vacuum::Update(float deltaTime)
    {
        InputManager& input = InputManager::GetInstance();
        PlayerAnimationManager* playerAnimationManager = PlayerAnimationManager::GetInstance();

        AudioManager::GetInstance().SetSoundPosition(blowingSound, GetOwner()->GetTransform()->GetPosition());
        AudioManager::GetInstance().SetSoundPosition(suckingSound, GetOwner()->GetTransform()->GetPosition());

        isShootingKeyPressed = input.IsKeyPressed(GLFW_KEY_2) || input.IsGamepadButtonPressed(GLFW_GAMEPAD_BUTTON_B);
        bool isSuccingKeyPressed = input.IsKeyPressed(GLFW_KEY_1) || input.IsGamepadButtonPressed(GLFW_GAMEPAD_BUTTON_A);

        if (isSuccingKeyPressed && volume < maxVolume)
        {
            if (static_cast<float>(glfwGetTime()) - lastSuckToggleTime > shootCooldown)
            {
                isSuccing = !isSuccing;
                isShooting = false;
                lastSuckToggleTime = static_cast<float>(glfwGetTime());
            }
        }
        else if (isShootingKeyPressed)
        {
            isShooting = true;
            isSuccing = false;
            playerAnimationManager->SetVacuumInactiveImmediate();
        }

        if (volume >= maxVolume)
        {
            isSuccing = false;
        }

        if (isSuccing)
        {
            if (!playerAnimationManager->isVacuumActive)
            {
                playerAnimationManager->SetVacuumActive();
                AudioManager::GetInstance().PlayAudio(suckingSound);
            }
        }
        else
        {
            if (playerAnimationManager->isVacuumActive)
            {
                playerAnimationManager->SetVacuumInactive();
                AudioManager::GetInstance().StopSound(suckingSound);
            }
        }

        float currentTime = static_cast<float>(glfwGetTime());

        for (auto it = pendingGravityDisables.begin(); it != pendingGravityDisables.end();)
        {
            if (currentTime >= it->disableTime)
            {
                if (it->entity && it->entity->GetComponent<Rigidbody>())
                {
                    it->entity->GetComponent<Rigidbody>()->hasGravity = false;
                }
                it = pendingGravityDisables.erase(it);
            }
            else
            {
                ++it;
            }
        }

        // --- STRZELANIE ---
        if (isShooting)
        {
            if (isShootingKeyPressed && !wasShootingKeyPressed && volume > 0)
            {
                Shoot();
                shootKeyHoldStartTime = currentTime;
                lastShootTime = currentTime;
                AudioManager::GetInstance().StopSound(blowingSound);
                AudioManager::GetInstance().PlayAudioWithRandomPitch(blowingSound, 0.8f, 1.2f);
            }
            else if (isShootingKeyPressed && volume > 0)
            {
                if (currentTime - lastShootTime >= shootCooldown)
                {
                    Shoot();
                    lastShootTime = currentTime;
                    AudioManager::GetInstance().StopSound(blowingSound);
                    AudioManager::GetInstance().PlayAudioWithRandomPitch(blowingSound, 0.8f, 1.5f);
                }
            }
        }

        // --- SSANIE ---
        if (volume <= maxVolume && isSuccing)
        {
            auto ownerTransform = this->GetOwner()->GetTransform();
            glm::vec3 forward = ownerTransform->GetForward();
            glm::vec3 position = ownerTransform->GetPosition() - forward * (size + 0.01f);
            std::vector<Engine::Collider*> entities = collider->SphereOverlap(position, size);

            for (auto* entityCollider : entities)
            {
                if (entityCollider->GetOwner()->GetComponent<Thrash>())
                {
                    int thrashSizeInt = static_cast<int>(entityCollider->GetOwner()->GetComponent<Thrash>()->GetSize());
                    if (volume + thrashSizeInt <= maxVolume)
                    {
                        if (entityCollider->GetOwner()->GetComponent<Engine::Rigidbody>())
                        {
                            glm::vec3 direction = position + forward * (size / 2.0f + 0.01f) -
                                                  entityCollider->GetOwner()->GetTransform()->GetPosition();
                            entityCollider->GetOwner()->GetComponent<Engine::Rigidbody>()->AddForce(
                                    direction, Engine::ForceMode::Force);
                            if (thrashSizeInt == 10)
                                playerAnimationManager->SuckBigObject();
                        }
                    }
                }

                if (entityCollider->GetOwner()->GetComponent<AiManager>())
                {
                    int slimeSizeInt = entityCollider->GetOwner()->GetComponent<AiManager>()->GetSize();
                    if (volume + slimeSizeInt <= maxVolume)
                    {
                        glm::vec3 direction = position + forward * (size / 2.0f + 0.01f) - entityCollider->GetOwner()->
                                              GetTransform()->GetPosition();
                        if (entityCollider->GetOwner()->GetComponent<Engine::Rigidbody>())
                        {
                            entityCollider->GetOwner()->GetComponent<Engine::Rigidbody>()->hasGravity = false;
                            entityCollider->GetOwner()->GetComponent<Engine::Rigidbody>()->AddForce(
                                    direction, Engine::ForceMode::Force);
                        }
                        if (slimeSizeInt == 10)
                            playerAnimationManager->SuckBigObject();
                    }
                }
            }

            glm::vec3 centerPosition = position + forward * (size / 2.0f);
            entities = collider->SphereOverlap(centerPosition, centerSize);
            for (auto* entityCollider : entities)
            {
                if (entityCollider->GetOwner()->GetComponent<Thrash>())
                {
                    int thrashSizeInt = static_cast<int>(entityCollider->GetOwner()->GetComponent<Thrash>()->GetSize());
                    if (volume + thrashSizeInt <= maxVolume)
                    {
                        items.push_back(entityCollider->GetOwner());
                        volume += thrashSizeInt;
                        UpdateFillIndicator();
                        if (entityCollider->GetOwner()->GetComponent<BoxCollider>())
                            entityCollider->GetOwner()->GetComponent<Engine::BoxCollider>()->SetTrigger(true);
                        if (entityCollider->GetOwner()->GetComponent<Engine::Rigidbody>())
                            entityCollider->GetOwner()->GetComponent<Engine::Rigidbody>()->hasGravity = false;
                        entityCollider->GetOwner()->GetTransform()->SetPosition(glm::vec3(1000, 1, 1000));
                    }
                }

                Entity* entity = entityCollider->GetOwner();
                if (entity->GetComponent<AiManager>())
                {
                    AiManager* slimeAi = entity->GetComponent<AiManager>();
                    int slimeSizeInt = slimeAi->GetSize();

                    if (volume < maxVolume)
                    {
                        entity->GetTransform()->SetPosition(glm::vec3(1000, 1, 1000));
                        volume += slimeSizeInt;
                        items.push_back(entityCollider->GetOwner());
                        UpdateFillIndicator();

                        Rigidbody* rb = entity->GetComponent<Rigidbody>();
                        if (rb)
                        {
                            rb->hasGravity = false;
                        }

                        BoxCollider* box = entity->GetComponent<BoxCollider>();
                        if (box)
                        {
                            box->SetTrigger(true);
                        }

                        slimeAi->SetEnabled(false);

                        if (slimeSizeInt == 10)
                            playerAnimationManager->SuckBigObject();
                    }
                }
            }
        }

        wasShootingKeyPressed = isShootingKeyPressed;

        ThrashManager::GetInstance()->VacuumCount = items.size();
        ThrashManager::GetInstance()->VacuumVolume = volume;
    }

    void Vacuum::Shoot()
    {
        if (!items.empty())
        {
            Engine::Entity* item = items.back();
            items.pop_back();

            int itemSize = 0;

            if (item->GetComponent<Thrash>())
            {
                itemSize = static_cast<int>(item->GetComponent<Thrash>()->GetSize());
            }
            else if (item->GetComponent<AiManager>())
            {
                itemSize = item->GetComponent<AiManager>()->GetSize();

                AiManager* ai = item->GetComponent<AiManager>();
                ai->SetEnabled(true);
            }
            else
            {
                return;
            }

            volume -= itemSize;
            UpdateFillIndicator();

            BoxCollider* collider = item->GetComponent<BoxCollider>();
            if (collider)
                collider->SetTrigger(false);

            Rigidbody* rb = item->GetComponent<Rigidbody>();
            if (rb)
            {
                rb->hasGravity = true;
                rb->angularVelocity = glm::vec3(0);

                //pendingGravityDisables.push_back({item, static_cast<float>(glfwGetTime()) + 2.0f});
            }

            glm::vec3 position = GetOwner()->GetTransform()->GetParent()->GetPosition();
            glm::vec3 forward = GetOwner()->GetTransform()->GetParent()->GetForward();
            item->GetTransform()->SetPosition(position + forward * (size + 0.5f));

            if (rb)
                rb->AddForce(forward * shootForce * float(itemSize), Engine::ForceMode::Force);

            if (itemSize == 20)
                PlayerAnimationManager::GetInstance()->ShootBigObject();

            PlayerAnimationManager::GetInstance()->PlayVacuumShotVfx();
        }
    }

    void Vacuum::UpdateFillIndicator()
    {
        PlayerAnimationManager* playerAnimationManager = PlayerAnimationManager::GetInstance();
        if (playerAnimationManager == nullptr)
        {
            return;
        }
        Materials::VacuumIndicatorMaterial* material = playerAnimationManager->StrapMaterial;
        if (material == nullptr)
        {
            return;
        }

        const float fill = static_cast<float>(volume) / static_cast<float>(maxVolume);
        material->SetFill(fill);

        if (fill < 0.5f)
        {
            material->SetEmissiveColor(glm::vec3(0.0f, 10.0f, 0.0f));
        }
        else if (fill < 0.75f)
        {
            material->SetEmissiveColor(glm::vec3(10.0f, 8.0f, 0.0f));
        }
        else if (fill < 0.99f)
        {
            material->SetEmissiveColor(glm::vec3(10.0f, 1.0f, 0.0f));
        }
        else
        {
            material->SetEmissiveColor(glm::vec3(10.0f, 0.0f, 0.0f));
        }
    }

    rapidjson::Value Vacuum::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        END_COMPONENT_SERIALIZATION
    }

    void Vacuum::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void Vacuum::DeserializeReferencesPass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
#if EDITOR
    void Vacuum::DrawImGui()
    {
    }
#endif
} // namespace Engine
