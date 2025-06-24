#include "SlimeAnimationManager.h"

using namespace Engine;

SlimeAnimationManager* SlimeAnimationManager::GetInstance()
{
    if (!Instance)
        Instance = new SlimeAnimationManager();
    return Instance;
}

void SlimeAnimationManager::SetSlimeIdleModel(AnimatedModelRenderer* slime)
{
    SlimeIdle = slime;
    if (SlimeIdle && SlimeIdle->GetAnimation())
        AnimationDuration = SlimeIdle->GetAnimation()->GetDuration();
}

void SlimeAnimationManager::SetSlimeWalkModel(AnimatedModelRenderer* slime)
{
    SlimeWalk = slime;
    if (SlimeWalk && SlimeWalk->GetAnimation())
        AnimationDuration = SlimeWalk->GetAnimation()->GetDuration();
}

void SlimeAnimationManager::PlayIdle()
{
    if (SlimeIdle)
    {
        SlimeIdle->GetAnimator().ResumeAnimation();
    }
}

void SlimeAnimationManager::PlayWalk()
{
    if (SlimeWalk)
    {
        SlimeWalk->GetAnimator().ResumeAnimation();
    }
}

void SlimeAnimationManager::StopAllAnimations()
{
    if (SlimeIdle != nullptr)
        SlimeIdle->GetAnimator().PauseAnimation();

    if (SlimeWalk != nullptr)
        SlimeWalk->GetAnimator().PauseAnimation();
}

float SlimeAnimationManager::GetTickForFrame(int frame, int totalFrames, float duration)
{
    return (frame / static_cast<float>(totalFrames)) * duration;
}
