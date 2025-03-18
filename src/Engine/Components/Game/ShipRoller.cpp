#include "ShipRoller.h"
#include "Engine/EngineObjects/Entity.h"
#include "GLFW/glfw3.h"

namespace Engine
{
    ShipRoller::ShipRoller(const glm::vec3& Amplitude, const glm::vec3& Velocity) :
        Amplitude(Amplitude), Velocity(Velocity)
    {
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
