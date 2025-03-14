//
// Created by Kacper on 28.01.2025.
//

#include "ShipRoller.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/EngineObjects/UpdateManager.h"
#include "GLFW/glfw3.h"

namespace Engine
{
    ShipRoller::ShipRoller(const glm::vec3& Amplitude, glm::vec3 Velocity) :
        Amplitude(Amplitude), Velocity(Velocity)
    {
    }

    ShipRoller::~ShipRoller()
    {
        UpdateManager::GetInstance()->UnregisterComponent(this);
    }

    void ShipRoller::OnAdd(Entity* NewOwner)
    {
        Component::OnAdd(NewOwner);
        UpdateManager::GetInstance()->RegisterComponent(this);
    }

    void ShipRoller::Update(float DeltaTime)
    {
        const float time = static_cast<float>(glfwGetTime());
        GetOwner()->GetTransform()->SetEulerAngles(glm::vec3(
                InitialRotation.x + sin(time * Velocity.x) * Amplitude.x,
                InitialRotation.y + cos(time * Velocity.y) * Amplitude.y,
                InitialRotation.z + sin(time * Velocity.z + 1.57f) * Amplitude.z));
    }
} // Engine
