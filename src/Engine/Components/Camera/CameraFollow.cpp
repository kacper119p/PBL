#include "CameraFollow.h"  
#include "../../EngineObjects/Entity.h"
  

void CameraFollow::Update(float deltaTime)
{
    if (!Camera || !target)
        return;
        auto targetTransform = target->GetTransform();
        glm::vec3 targetPosition = targetTransform->GetPosition();
        glm::vec3 desiredPosition = targetPosition + PositionOffset;

        // Smooth follow using linear interpolation
        glm::vec3 currentPosition = Camera->GetPosition();
        glm::vec3 newPosition = glm::mix(currentPosition, desiredPosition, deltaTime * 5.0f);
        Camera->SetPosition(newPosition);
}
