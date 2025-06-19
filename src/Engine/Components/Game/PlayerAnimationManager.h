#pragma once
#include "Engine/Components/Renderers/AnimatedModelRenderer.h"
namespace Engine
{
    class PlayerAnimationManager
    {
    private:
        static inline PlayerAnimationManager* Instance{nullptr};

    public:
        AnimatedModelRenderer* TrackLeft = nullptr;
        AnimatedModelRenderer* TrackRight = nullptr;
        AnimatedModelRenderer* HandLeft = nullptr;
        AnimatedModelRenderer* HandRight = nullptr;

    
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
    };
}
