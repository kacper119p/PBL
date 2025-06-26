#include "CleaningGameMode.h"

#include "Engine/EngineObjects/UpdateManager.h"
#include "Engine/EngineObjects/Scene/Scene.h"
#include "Engine/Components/BloodSystem/BloodManager.h"
#include "Engine/Components/Game/ThrashManager.h"
#include "Engine/Input/InputManager.h"
#include "Engine/EngineObjects/Scene/SceneManager.h"
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

        ThrashManager::GetInstance()->SetIsCurrentLevelCompleted(false);
        ThrashManager::GetInstance()->ResetTasks();
    }

    void CleaningGameMode::Update(const float DeltaTime)
    {
        if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_P))
        {
            switch (ThrashManager::GetInstance()->GetCurrentLevel())
            {
                case 0:
                {
                    SceneManager::ChangeScene("./res/scenes/MainMenu.lvl");
                }
                break;
                case 1:
                {
                    SceneManager::ChangeScene("./res/scenes/Gameplay1.lvl");
                }
                break;
                case 2:
                {
                    SceneManager::ChangeScene("./res/scenes/Gameplay.lvl");
                }
                break;
            }
        }

        if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_O))
        {
            ThrashManager::GetInstance()->SetIsCurrentLevelCompleted(true);
        }

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
            
                switch (ThrashManager::GetInstance()->GetCurrentLevel())
                {
                    case 1:
                    {
                        ThrashManager::GetInstance()->SetLevelEndTime(glfwGetTime());
                        ThrashManager::GetInstance()->SetIsCurrentLevelCompleted(true);
                    }
                    break;
                    case 2:
                    {
                        ThrashManager::GetInstance()->SetLevelEndTime(glfwGetTime());
                        ThrashManager::GetInstance()->SetIsCurrentLevelCompleted(true);
                    }
                    break;
                }

                if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_E) ||
                    InputManager::GetInstance().IsGamepadButtonPressed(GLFW_GAMEPAD_BUTTON_A))
                {
                    switch (ThrashManager::GetInstance()->GetCurrentLevel())
                    {
                        case 0:
                        {
                            SceneManager::ChangeScene("./res/scenes/Gameplay1.lvl");
                            ThrashManager::GetInstance()->SetCurrentLevel(
                            ThrashManager::GetInstance()->GetCurrentLevel() + 1);
                        }
                        break;
                        case 1:
                        {
                            SceneManager::ChangeScene("./res/scenes/Gameplay.lvl");
                            ThrashManager::GetInstance()->SetCurrentLevel(ThrashManager::GetInstance()->GetCurrentLevel() + 1);
                        }
                        break;
                        case 2:
                        {
                            SceneManager::ChangeScene("./res/scenes/MainMenu.lvl");
                            ThrashManager::GetInstance()->SetCurrentLevel(0);
                        }
                        break;
                    }

                }
            
        }
    }
    bool CleaningGameMode::isLevelFinished()
    {
        if (ThrashManager::GetInstance()->GetCurrentLevel() == 2)
        {   
            ThrashManager* thrashManager = ThrashManager::GetInstance();
            if (cleanedPercent >= 99.0f && thrashManager->GetThrashCount() == 0 && thrashManager->cleanedBooks() && thrashManager->cleanedCoins() && thrashManager->cleanedWeapons())
            {
                return true;
                ThrashManager::GetInstance()->SetIsCurrentLevelCompleted(true);
            }
            else
            {
                return false;
                ThrashManager::GetInstance()->SetIsCurrentLevelCompleted(false);
            }
        }
        else
        {
            if (ThrashManager::GetInstance()->GetThrashCount() == 0)
            {
                return true;
                ThrashManager::GetInstance()->SetIsCurrentLevelCompleted(true);
            }
            else
            {
                return false;
                ThrashManager::GetInstance()->SetIsCurrentLevelCompleted(false);
            }
        }
    }
} // namespace Engine
