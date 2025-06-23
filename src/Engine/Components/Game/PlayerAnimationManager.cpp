#include "PlayerAnimationManager.h"

#include "Materials/VacuumIndicatorMaterial.h"

void Engine::PlayerAnimationManager::StopAllAnimations()
{
    if (TrackLeft != nullptr)
        TrackLeft->GetAnimator().PauseAnimation();
    if (TrackRight != nullptr)
        TrackRight->GetAnimator().PauseAnimation();

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
{
    TrackLeft->GetAnimator().PauseAnimation();
}

void Engine::PlayerAnimationManager::TrackRightStop()
{
    TrackRight->GetAnimator().PauseAnimation();
}

void Engine::PlayerAnimationManager::SetHandLeft(AnimatedModelRenderer* handLeft)
{
    HandLeft = handLeft;
    LeftHandAnimationDuration = handLeft->GetAnimation()->GetDuration();
    HandLeft->GetAnimator().PauseAnimation();
}

void Engine::PlayerAnimationManager::SetHandRight(AnimatedModelRenderer* handRight)
{
    HandRight = handRight;
    RightHandAnimationDuration = handRight->GetAnimation()->GetDuration();
    HandRight->GetAnimator().PauseAnimation();
}

void Engine::PlayerAnimationManager::SetVacuumVfx(VacuumVfx* Vfx)
{
    vacuumVfx = Vfx;
    isVacuumActive = false;
}

void Engine::PlayerAnimationManager::SetVacuumShotVfx(VacuumShotVfx* ShotVfx)
{
    vacuumShotVfx = ShotVfx;
}

void Engine::PlayerAnimationManager::SetVacuumActive()
{
    if (vacuumVfx != nullptr)
    {
        vacuumVfx->Activate();
        isVacuumActive = true;
    }
}

void Engine::PlayerAnimationManager::SetVacuumInactive()
{
    if (vacuumVfx != nullptr)
    {
        vacuumVfx->Deactivate();
        isVacuumActive = false;
    }
}

void Engine::PlayerAnimationManager::SetVacuumFront(AnimatedModelRenderer* Front)
{
    VacuumFront = Front;
    if (VacuumFront != nullptr)
    {
        VacuumFront->GetAnimator().PauseAnimation();
    }
}

void Engine::PlayerAnimationManager::ChangeLeftHandPosition(int newPosition)
{
    float startTick = GetTickForFrame(LeftHandPosition * 10, 20, LeftHandAnimationDuration);
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

void Engine::PlayerAnimationManager::SuckBigObject()
{
    if (VacuumFront != nullptr)
    {
        VacuumFront->GetAnimator().PlayOnceFromTo(GetTickForFrame(0, 20, LeftHandAnimationDuration),
                                                  GetTickForFrame(20, 20, LeftHandAnimationDuration));
    }
}

void Engine::PlayerAnimationManager::ShootBigObject()
{
    if (VacuumFront != nullptr)
    {
        VacuumFront->GetAnimator().PlayOnceFromTo(GetTickForFrame(20, 20, LeftHandAnimationDuration),
                                                  GetTickForFrame(0, 20, LeftHandAnimationDuration));
    }
}

void Engine::PlayerAnimationManager::PlayVacuumShotVfx()
{
    if (vacuumShotVfx != nullptr)
    {
        vacuumShotVfx->Play();
    }

}

void Engine::PlayerAnimationManager::SetIndicator(float Fill, const glm::vec3& Color)
{
    if (StrapMaterial == nullptr)
    {
        return;
    }

    StrapMaterial->SetFill(Fill);
    StrapMaterial->SetEmissiveColor(Color);
}
