#include "PlayerAnimationManager.h"

void Engine::PlayerAnimationManager::StopAllAnimations() 
{ 
    if (TrackLeft!=nullptr)
	TrackLeft->GetAnimator().PauseAnimation();
    if (TrackRight != nullptr)
    TrackRight->GetAnimator().PauseAnimation();
    /*HandLeft->GetAnimator().PauseAnimation();
    HandRight->GetAnimator().PauseAnimation();*/

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
