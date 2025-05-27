#include "UpdateManager.h"

#include "GameMode/GameMode.h"

namespace Engine
{
    UpdateManager* UpdateManager::Instance = nullptr;

    UpdateManager::UpdateManager() = default;

    void UpdateManager::Initialize()
    {
        Instance = new UpdateManager;
    }

    void UpdateManager::Update(const float DeltaTime)
    {
        if (!Dead.empty())
        {
            for (IUpdateable* component : Dead)
            {
                std::erase(Updateables, component);
            }
            Dead.clear();
        }

        if (GameMode)
        {
            GameMode->Update(DeltaTime);
        }

        for (IUpdateable* component : Updateables)
        {
            component->Update(DeltaTime);
        }
    }
} // Engine
