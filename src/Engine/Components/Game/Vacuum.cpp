#include "Vacuum.h"
#include "Engine/EngineObjects/Entity.h"
#include "Thrash.h"
#include "Engine/EngineObjects/UpdateManager.h"
#include "Engine/Components/Physics/Rigidbody.h"
#include "Engine/EngineObjects/Scene/Scene.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Components/Colliders/SphereCollider.h"
#include "Engine/Components/Game/ThrashManager.h"
#include <iostream>

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
        Engine::UpdateManager::GetInstance()->RegisterComponent(this);
    }

    void Vacuum::Update(float deltaTime)
    {
        InputManager& input = InputManager::GetInstance();

        isShootingKeyPressed = input.IsKeyPressed(GLFW_KEY_2);
        bool isSuccingKeyPressed = input.IsKeyPressed(GLFW_KEY_1);

        if (isSuccingKeyPressed)
        {
            isSuccing = true;
            isShooting = false;
        }
        else if (isShootingKeyPressed)
        {
            isShooting = true;
            isSuccing = false;
        }

        float currentTime = static_cast<float>(glfwGetTime());

        // --- STRZELANIE ---
        if (isShooting)
        {
            if (isShootingKeyPressed && !wasShootingKeyPressed)
            {
                Shoot();
                shootKeyHoldStartTime = currentTime;
                lastShootTime = currentTime;
            }
            else if (isShootingKeyPressed)
            {
                if (currentTime - lastShootTime >= shootCooldown)
                {
                    Shoot();
                    lastShootTime = currentTime;
                }
            }
        }

        // --- SSANIE ---
        if (volume <= maxVolume && isSuccing)
        {
            auto position = this->GetOwner()->GetTransform()->GetPosition();
            std::vector<Engine::Collider*> entities = collider->SphereOverlap(position, size);

            for (auto* entityCollider : entities)
            {
                if (entityCollider->GetOwner()->GetComponent<Thrash>())
                {
                    int thrashSizeInt = static_cast<int>(entityCollider->GetOwner()->GetComponent<Thrash>()->GetSize());
                    if (volume + thrashSizeInt <= maxVolume)
                    {
                        glm::vec3 direction = position - entityCollider->GetOwner()->GetTransform()->GetPosition();
                        entityCollider->GetOwner()->GetComponent<Engine::Rigidbody>()->AddForce(
                                direction, Engine::ForceMode::Force);
                    }
                }
            }

            entities = collider->SphereOverlap(position, centerSize);
            for (auto* entityCollider : entities)
            {
                if (entityCollider->GetOwner()->GetComponent<Thrash>())
                {
                    int thrashSizeInt = static_cast<int>(entityCollider->GetOwner()->GetComponent<Thrash>()->GetSize());
                    if (volume + thrashSizeInt <= maxVolume)
                    {
                        items.push_back(entityCollider->GetOwner());
                        volume += thrashSizeInt;
                        entityCollider->GetOwner()->GetComponent<Engine::BoxCollider>()->SetTrigger(true);
                        entityCollider->GetOwner()->GetComponent<Engine::Rigidbody>()->hasGravity = false;
                        entityCollider->GetOwner()->GetTransform()->SetPosition(glm::vec3(1000, 1, 1000));
                    }
                }
            }
        }

        wasShootingKeyPressed = isShootingKeyPressed;

        ThrashManager::GetInstance()->VacuumCount = items.size();
        ThrashManager::GetInstance()->VacuumVolume = volume;

        // TODO: remove comment
        /*std::cout << "Vacuum Volume: " << volume << ", Vacuum count: " << items.size() << std::endl;
        std::cout << "Thrash count: " << ThrashManager::GetInstance()->GetThrashCount() << std::endl;*/
    }

    void Vacuum::Shoot()
    {
        if (!items.empty())
        {
            Engine::Entity* item = items.back();
            items.pop_back();
            int thrashSizeInt = static_cast<int>(item->GetComponent<Thrash>()->GetSize());
            volume -= thrashSizeInt;

            item->GetComponent<Engine::BoxCollider>()->SetTrigger(false);

            glm::vec3 position = GetOwner()->GetTransform()->GetParent()->GetPosition();
            glm::vec3 forward = GetOwner()->GetTransform()->GetParent()->GetForward();
            item->GetTransform()->SetPosition((position + forward)+glm::vec3(0,1,0)); 
            item->GetComponent<Engine::Rigidbody>()->angularVelocity.y=0.0f;
            item->GetComponent<Engine::Rigidbody>()->hasGravity = true;
            item->GetComponent<Engine::Rigidbody>()->AddForce(forward * 100.0f, Engine::ForceMode::Force);
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
    void Vacuum::DrawImGui() {}
#endif
} // namespace Engine
