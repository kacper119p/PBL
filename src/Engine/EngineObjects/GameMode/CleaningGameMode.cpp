#include "CleaningGameMode.h"

#include "Engine/EngineObjects/UpdateManager.h"
#include "Engine/EngineObjects/Scene/Scene.h"

namespace Engine
{
    CleaningGameMode::CleaningGameMode() :
        BloodManager()
    {

    }

    CleaningGameMode::~CleaningGameMode()
    {
        UpdateManager::GetInstance()->UnregisterGameMode(this);
        delete BloodManager;
    }

    void CleaningGameMode::Start()
    {
        UpdateManager::GetInstance()->RegisterGameMode(this);
        BloodManager = new class BloodManager(GetScene()->GetBounds());
    }

    void CleaningGameMode::Update(const float DeltaTime)
    {
        BloodManager->Update(DeltaTime);
    }
}
