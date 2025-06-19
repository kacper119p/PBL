#include "PlayerAnimationManager.h"

void Engine::PlayerAnimationManager::StopAllAnimations() 
{ 
    if (TrackLeft!=nullptr)
	TrackLeft->GetAnimator().PauseAnimation();
    if (TrackRight != nullptr)
    TrackRight->GetAnimator().PauseAnimation();
    HandLeft->GetAnimator().PauseAnimation();
    HandRight->GetAnimator().PauseAnimation();

}

void Engine::PlayerAnimationManager::TrackLeftForward() 
{ 
    TrackLeft->GetAnimator().SetPlayBackward(false);
    TrackLeft->GetAnimator().ResumeAnimation();
}

void Engine::PlayerAnimationManager::TrackLeftBackward() 
{ 
    TrackLeft->GetAnimator().SetPlayBackward(true);
    TrackLeft->GetAnimator().ResumeAnimation();
}

void Engine::PlayerAnimationManager::TrackRightForward() 
{
    TrackRight->GetAnimator().SetPlayBackward(false);
    TrackRight->GetAnimator().ResumeAnimation();
}

void Engine::PlayerAnimationManager::TrackRightBackward() 
{
    TrackRight->GetAnimator().SetPlayBackward(true);
    TrackRight->GetAnimator().ResumeAnimation();
}

void Engine::PlayerAnimationManager::TrackLeftStop() 
{ TrackLeft->GetAnimator().PauseAnimation(); }

void Engine::PlayerAnimationManager::TrackRightStop() 
{ TrackRight->GetAnimator().PauseAnimation(); }

void Engine::PlayerAnimationManager::ChangeLeftHandPosition(int newPosition) 
{ 
    float startTick = GetTickForFrame(LeftHandPosition*10, 20, LeftHandAnimationDuration);
    float endTick = GetTickForFrame(newPosition * 10, 20, LeftHandAnimationDuration);
    HandLeft->GetAnimator().PlayOnceFromTo(startTick, endTick);
    LeftHandPosition = newPosition;
}

void Engine::PlayerAnimationManager::ChangeRightHandPosition(int newPosition) 
{
    float startTick = GetTickForFrame(RightHandPosition * 10, 20, RightHandAnimationDuration);
    float endTick = GetTickForFrame(newPosition * 10, 20, RightHandAnimationDuration);
    HandRight->GetAnimator().PlayOnceFromTo(startTick, endTick);
    RightHandPosition = newPosition;
}
