#include "Camera.h"

namespace Engine
{
    Camera::Camera(const glm::mat4& projectionMatrix, const glm::float32 sensitivity) :
        projectionMatrix(projectionMatrix),
        sensitivity(sensitivity)
    {
        SetRotation(pitch, yaw);
    }
} // Engine
