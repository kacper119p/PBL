#include "RigidbodyUpdateManager.h"
#include <algorithm>

namespace Engine
{
    RigidbodyUpdateManager* RigidbodyUpdateManager::Instance = nullptr;

    RigidbodyUpdateManager::RigidbodyUpdateManager() = default;

    void RigidbodyUpdateManager::Initialize()
    {
        if (!Instance)
        {
            Instance = new RigidbodyUpdateManager();
        }
    }

    void RigidbodyUpdateManager::Update(float DeltaTime)
    {
        for (Rigidbody* rigidbody : Updateables)
        {
            if (rigidbody)
            {
                rigidbody->Update(DeltaTime);
            }
        }

        for (Rigidbody* rigidbody : Dead)
        {
            std::erase(Updateables, rigidbody);
        }
        Dead.clear();
    }
} // namespace Engine
