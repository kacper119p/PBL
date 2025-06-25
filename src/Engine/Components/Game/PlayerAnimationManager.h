#pragma once
#include "Engine/Components/Renderers/AnimatedModelRenderer.h"
#include "Engine/Components/VFX/VacuumVfx.h"
#include "Engine/Components/VFX/VacuumShotVfx.h"
#include "Materials/Material.h"

namespace Materials
{
    class VacuumIndicatorMaterial;
}

namespace Engine
{
    class PlayerAnimationManager
    {
    private:
        static inline PlayerAnimationManager* Instance{nullptr};
        int HandAnimationFrames = 20;
        float LeftHandAnimationDuration;
        float RightHandAnimationDuration;
        AnimatedModelRenderer* HandLeft = nullptr;
        AnimatedModelRenderer* HandRight = nullptr;
        AnimatedModelRenderer* VacuumFront = nullptr;

        VacuumVfx* vacuumVfx = nullptr;
        VacuumShotVfx* vacuumShotVfx = nullptr;

    public:
        AnimatedModelRenderer* TrackLeft = nullptr;
        AnimatedModelRenderer* TrackRight = nullptr;

        Materials::VacuumIndicatorMaterial* StrapMaterial = nullptr;

        bool isVacuumActive = false;

        int LeftHandPosition = 0;
        int RightHandPosition = 0;

        PlayerAnimationManager() = default;

        ~PlayerAnimationManager() = default;

        static PlayerAnimationManager* GetInstance()
        {
            if (!Instance)
            {
                Instance = new PlayerAnimationManager();
            }
            return Instance;
        }

        void StopAllAnimations();

        void TrackLeftForward();

        void TrackLeftBackward();

        void TrackRightForward();

        void TrackRightBackward();

        void TrackLeftStop();

        void TrackRightStop();

        void SetHandLeft(AnimatedModelRenderer* handLeft);

        void SetHandRight(AnimatedModelRenderer* handRight);

        void SetVacuumVfx(VacuumVfx* Vfx);

        void SetVacuumShotVfx(VacuumShotVfx* ShotVfx);

        void SetVacuumActive();

        void SetVacuumInactive();

        void SetVacuumInactiveImmediate();

        void SetVacuumFront(AnimatedModelRenderer* Front);

        AnimatedModelRenderer* GetVacuumFront() const { return VacuumFront; };

        float GetTickForFrame(int frame, int totalFrames, float duration)
        {
            return (frame / static_cast<float>(totalFrames)) * duration;
        }

        void ChangeLeftHandPosition(int newPosition);

        void ChangeRightHandPosition(int newPosition);

        void SuckBigObject();

        void ShootBigObject();

        void PlayVacuumShotVfx();

        void SetIndicator(float Fill, const glm::vec3& Color);

        void SetLeftTackSpeed(float speed);

        void SetRightTackSpeed(float speed);

    };
}
