#include "CleaningGameMode.h"

#include "Engine/EngineObjects/UpdateManager.h"
#include "Engine/EngineObjects/Scene/Scene.h"
#include "Engine/Components/BloodSystem/BloodManager.h"
#include "Engine/Components/Game/ThrashManager.h"
#include "GLFW/glfw3.h"

namespace Engine
{
    CleaningGameMode::CleaningGameMode() : BloodManager() {}

    CleaningGameMode::~CleaningGameMode()
    {
        UpdateManager::GetInstance()->UnregisterGameMode(this);
        delete BloodManager;
    }

    void CleaningGameMode::Start()
    {
        UpdateManager::GetInstance()->RegisterGameMode(this);
        BloodManager = new class BloodManager(GetScene()->GetBounds());
        ThrashManager::GetInstance()->SetLevelStartTime(glfwGetTime());
        //ThrashManager::GetInstance()->SetIsCurrentLevelCompleted(false);
    }

    void CleaningGameMode::Update(const float DeltaTime)
    {
        BloodManager->Update(DeltaTime);
        int refThrashCount = ThrashManager::GetInstance()->GetThrashCount();
        if (refThrashCount > ThrashCount)
        {
            ThrashCount = refThrashCount;
        }
        float refBloodFill = BloodManager->GetBloodFill();
        if (refBloodFill > BloodFill)
        {
            BloodFill = refBloodFill;
        }

        cleanedPercent = (1.0f - refBloodFill / BloodFill) * 100.0f;

        if (isLevelFinished())
        {
            int dupa = 3;
        }
    }
    bool CleaningGameMode::isLevelFinished()
    {
        if (cleanedPercent >= 99.0f  && ThrashManager::GetInstance()->GetThrashCount() == 0)
        {
            return true;
            //ThrashManager::GetInstance()->SetIsCurrentLevelCompleted(true);
        }
        else
        {
            return false;
            //ThrashManager::GetInstance()->SetIsCurrentLevelCompleted(false);
        }
    }
} // namespace Engine
