#pragma once  

#include "../Component.h"  
#include "../Transform.h"  
#include "../Updateable.h"  
#include "../../EngineObjects/UpdateManager.h"  
#include "../../EngineObjects/Camera.h"
#include "Serialization/SerializationUtility.h"

class CameraFollow
{  
public:
 static CameraFollow& GetInstance()
 {
     static CameraFollow instance;
     return instance;
 }

 CameraFollow(const CameraFollow&) = delete;
 CameraFollow& operator=(const CameraFollow&) = delete;

 Engine::Camera* Camera = nullptr;
 glm::vec3 PositionOffset = glm::vec3(6.0f, 6.0f, 6.0f);
 glm::vec3 EulerAngles = glm::vec3(0.0f, 0.0f, 0.0f);

 Engine::Entity* target = nullptr;

 void Update(float deltaTime);
 void SetCamera(Engine::Camera* Camera) { this->Camera = Camera; 
 Camera->SetYaw(45.0f);
 Camera->SetPitch(60.0f);
 };
 void SetTarget(Engine::Entity* target) { this->target = target; }

public:
 CameraFollow() = default;
};
