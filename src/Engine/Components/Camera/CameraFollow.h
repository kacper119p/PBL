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
 glm::vec3 PositionOffset = glm::vec3(6.0f, 10.0f, 6.0f);
 glm::vec3 EulerAngles = glm::vec3(0.0f, 0.0f, 0.0f);

 Engine::Entity* target = nullptr;

 void Update(float deltaTime);
 void SetCamera(Engine::Camera* Camera) { this->Camera = Camera; 
 Camera->SetYaw(38.5f);
 Camera->SetPitch(60.5f);
 };
 void SetTarget(Engine::Entity* target) { this->target = target; }

public:
 CameraFollow() = default;

 #if EDITOR
 void DrawImGui()
 {
     ImGui::Text("Camera Follow Settings");
     ImGui::DragFloat3("Position Offset", &PositionOffset.x, 0.1f);
     ImGui::DragFloat3("Euler Angles", &EulerAngles.x, 0.1f);

     float yaw = Camera ? Camera->GetYaw() : 0.0f;
     float pitch = Camera ? Camera->GetPitch() : 0.0f;
     bool changed = false;

     changed |= ImGui::DragFloat("Yaw", &yaw, 0.5f);
     changed |= ImGui::DragFloat("Pitch", &pitch, 0.5f);

     if (changed && Camera)
     {
         Camera->SetYaw(yaw);
         Camera->SetPitch(pitch);
     }

     if (ImGui::Button("Reset Camera"))
     {
         if (Camera)
         {
             Camera->SetPosition(glm::vec3(0.0f, 5.0f, 20.0f));
             Camera->SetYaw(38.5f);
             Camera->SetPitch(60.5f);
         }
     }
 }
 #endif
};
