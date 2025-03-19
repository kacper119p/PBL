#include "Rotator.h"

namespace Engine
{
    Rotator::Rotator(const glm::vec3& Velocity) :
        Velocity(Velocity)
    {
    }

    void Rotator::Update(float DeltaTime)
    {
        GetOwner()->GetTransform()->SetEulerAngles(GetOwner()->GetTransform()->GetEulerAngles() + Velocity * DeltaTime);
    }
} // Engine
