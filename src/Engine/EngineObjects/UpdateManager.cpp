#include "UpdateManager.h"

namespace Engine
{
    UpdateManager* UpdateManager::Instance = nullptr;

    UpdateManager::UpdateManager() = default;

    void UpdateManager::Initialize()
    {
        Instance = new UpdateManager;
    }

    void UpdateManager::Update(float DeltaTime)
    {
        if (!Dead.empty())
        {
            for (IUpdateable* component : Dead)
            {
                std::erase(Updateables, component);
            }
            Dead.clear();
        }

        for (IUpdateable* component : Updateables)
        {
            component->Update(DeltaTime);
        }
    }
} // Engine
