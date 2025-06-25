#include <glm/glm.hpp>
#include <iostream>
#include "Engine/Components/Component.h"
#include "Engine/Components/Transform.h"
#include "Engine/Input/InputManager.h"
#include "../Interfaces/IUpdateable.h"
#include "Serialization/SerializationUtility.h"
#include "Engine/EngineObjects/Entity.h"
#include "../../EngineObjects/UpdateManager.h"
#include "Engine/Components/Physics/Rigidbody.h"
#include "Audio/AudioManager.h"

namespace Engine
{
    class MovementComponent : public Component, public IUpdateable
    {
    private:
        float Speed = 120.0f;
        float RotationSpeed = 40.0f;
        float BothRotationSpeed = RotationSpeed * 3.0f;
        float CurrentVelocity = 0.0f;
        float smooth = 0.97f;

        float LeftHandChangeTime = -20.0f;
        float RightHandChangeTime = -20.0f;

        std::shared_ptr<ma_sound> DrivingSound;
        std::shared_ptr<ma_sound> BloodCleaningSound;

    public:
        MovementComponent()
        {
            UpdateManager::GetInstance()->RegisterComponent(this);
        }
        ~MovementComponent() override
        {
            UpdateManager::GetInstance()->UnregisterComponent(this);
        }

        void Start() override
        {
            GetOwner()->GetComponent<Rigidbody>()->constraints.freezeRotationX = true;
            GetOwner()->GetComponent<Rigidbody>()->constraints.freezeRotationZ = true;
            DrivingSound = AudioManager::GetInstance().CreateSoundInstance("jezdzenie");
            AudioManager::GetInstance().SetLooping(DrivingSound, true);
            AudioManager::GetInstance().SetVolume(DrivingSound, 6.5f);
            BloodCleaningSound = AudioManager::GetInstance().CreateSoundInstance("mop");
            AudioManager::GetInstance().SetVolume(BloodCleaningSound, 20.0f);
        }

        void Update(float deltaTime) override;

        void OnDestroy() override
        {
        }

        bool CanChangeLeftHand();

        bool CanChangeRightHand();

        SERIALIZATION_EXPORT_CLASS(MovementComponent);


#if EDITOR
        void DrawImGui() override
        {
            char speedBuffer[32];
            snprintf(speedBuffer, sizeof(speedBuffer), "%.2f", Speed);
            ImGui::Text("MovementComponent: Speed = %s", speedBuffer);
        }
#endif
    };
} // namespace Engine
