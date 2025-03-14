//
// Created by Kacper on 28.01.2025.
//

#include "Rotator.h"
#include "Engine/EngineObjects/UpdateManager.h"

namespace Engine
{
    Rotator::Rotator(const glm::vec3& Velocity) :
        Velocity(Velocity)
    {
    }

    Rotator::~Rotator()
    {
        UpdateManager::GetInstance()->UnregisterComponent(this);
    }

    void Rotator::OnAdd(Entity* NewOwner)
    {
        Component::OnAdd(NewOwner);
        UpdateManager::GetInstance()->RegisterComponent(this);
    }

    void Rotator::Update(float DeltaTime)
    {
        GetOwner()->GetTransform()->SetEulerAngles(GetOwner()->GetTransform()->GetEulerAngles() + Velocity * DeltaTime);
    }
} // Engine
