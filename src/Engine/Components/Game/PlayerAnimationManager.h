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
        float GetTickForFrame(int frame, int totalFrames, float duration)
        {
            return (frame / static_cast<float>(totalFrames)) * duration;
        }
        void ChangeLeftHandPosition(int newPosition);
        void ChangeRightHandPosition(int newPosition);

    };
}
