#pragma once

#include "Engine/Components/Renderers/AnimatedModelRenderer.h"

namespace Engine
{
    class SlimeAnimationManager
    {
    private:
        static inline SlimeAnimationManager* Instance{nullptr};
        AnimatedModelRenderer* SlimeIdle = nullptr;
        AnimatedModelRenderer* SlimeWalk = nullptr;
        float AnimationDuration = 0.0f;

    public:
        SlimeAnimationManager() = default;

        ~SlimeAnimationManager() = default;

        static SlimeAnimationManager* GetInstance();

        void SetSlimeIdleModel(AnimatedModelRenderer* Slime);

        void SetSlimeWalkModel(AnimatedModelRenderer* Slime);

        AnimatedModelRenderer* GetSlimeIdleModel() { return SlimeIdle; }

        AnimatedModelRenderer* GetSlimeWalkModel() { return SlimeWalk; }

        void PlayIdle();

        void PlayWalk();

        void StopAllAnimations();

    private:
        float GetTickForFrame(int Frame, int TotalFrames, float Duration);
    };
}
