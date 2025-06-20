#pragma once
#include "Engine/Components/Renderers/AnimatedModelRenderer.h"
#include "Engine/Components/VFX/VacuumVfx.h"
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
        
        VacuumVfx* vacuumVfx = nullptr;
    public:
        AnimatedModelRenderer* TrackLeft = nullptr;
        AnimatedModelRenderer* TrackRight = nullptr;

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
        void SetHandLeft(AnimatedModelRenderer* handLeft)
        {
            HandLeft = handLeft;
            LeftHandAnimationDuration = handLeft->GetAnimation()->GetDuration();
            HandLeft->GetAnimator().PauseAnimation();
        }
        void SetHandRight(AnimatedModelRenderer* handRight)
        {
            HandRight = handRight;
            RightHandAnimationDuration = handRight->GetAnimation()->GetDuration();
            HandRight->GetAnimator().PauseAnimation();
        }
        void SetVacuumVfx(VacuumVfx* Vfx) 
        { 
            vacuumVfx = Vfx; 
            isVacuumActive = false;

        }
        void SetVacuumActive() 
        { 
            if (vacuumVfx!=nullptr) 
            {
                vacuumVfx->Activate();
                isVacuumActive = true;
            }
        }
        void SetVacuumInactive()
        {
            if (vacuumVfx != nullptr)
            {
                vacuumVfx->Deactivate();
                isVacuumActive = false;
            }
        }
        float GetTickForFrame(int frame, int totalFrames, float duration)
        {
            return (frame / static_cast<float>(totalFrames)) * duration;
        }
        void ChangeLeftHandPosition(int newPosition);
        void ChangeRightHandPosition(int newPosition);

    };
}
