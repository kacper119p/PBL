#pragma once
#include "Engine/Components/Renderers/AnimatedModelRenderer.h"
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
    public:
        AnimatedModelRenderer* TrackLeft = nullptr;
        AnimatedModelRenderer* TrackRight = nullptr;
        

        int LeftHandPosition = 1;
        int RightHandPosition = 1;

        bool CanChangeLeftHandPosition() const;
        bool CanChangeRightHandPosition() const;

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
        }
        void SetHandRight(AnimatedModelRenderer* handRight)
        {
            HandRight = handRight;
            RightHandAnimationDuration = handRight->GetAnimation()->GetDuration();
        }
        float GetTickForFrame(int frame, int totalFrames, float duration)
        {
            return (frame / static_cast<float>(totalFrames)) * duration;
        }
        void ChangeLeftHandPosition(int newPosition);
        void ChangeRightHandPosition(int newPosition);
        bool CanChangeLeftHandAnimation() { return HandLeft->GetAnimator().HasFinishedOnce(); }
        bool CanChangeRightHandAnimation() { return HandRight->GetAnimator().HasFinishedOnce(); }



    };
}
