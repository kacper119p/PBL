#include "CollisionUpdateManager.h"

namespace Engine
{
    CollisionUpdateManager* CollisionUpdateManager::Instance = nullptr;

    CollisionUpdateManager::CollisionUpdateManager() = default;

    void CollisionUpdateManager::Initialize() { Instance = new CollisionUpdateManager; }

    void CollisionUpdateManager::Update(float DeltaTime)
    {
        if (!Dead.empty())
        {
            for (Collider* component : Dead)
            {
                std::erase(Updateables, component);
            }
            Dead.clear();
        }

        for (Collider* component : Updateables)
        {
            if (component!=nullptr)
            {
                component->Update(DeltaTime);
            }
        }
    }
} // namespace Engine
