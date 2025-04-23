#pragma once
#include <vector>
namespace Engine
{
    class Collider;
    class ColliderManager
    {
    private:
        static ColliderManager* Instance;

        std::vector<Collider*> Colliders;
    public:
        ColliderManager() = default;
        static void Initialize();
        static ColliderManager* GetInstance() { return Instance; }
        inline void RegisterCollider(Collider* Collider) { Colliders.push_back(Collider); }
        inline void UnregisterCollider(Collider* Collider) { std::erase(Colliders, Collider); }
        inline void UnregisterColliderImmediate(Collider* Collider) { std::erase(Colliders, Collider); }

    };
} // namespace Engine
