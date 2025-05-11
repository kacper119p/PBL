#pragma once

#include <vector>
#include "Engine/Components/Colliders/Collider.h"
namespace Engine
{
    /**
     * @brief Singleton responsible for ticking updateable Colliders.
     */

    class CollisionUpdateManager
    {
    private:
        static CollisionUpdateManager* Instance;

        std::vector<Collider*> Updateables;
        std::vector<Collider*> Dead;

    private:
        CollisionUpdateManager();

    public:
        /**
         * @brief Initializes a new CollisionUpdateManager.
         */
        static void Initialize();

        /**
         * @brief Returns instance of the CollisionUpdateManager.
         */
        static CollisionUpdateManager* GetInstance() { return Instance; }

        /**
         * @brief Registers new updateable Collider to be ticked.
         * @param Collider Collider to be registered.
         */
        inline void RegisterCollider(Collider* Collider) { Updateables.push_back(Collider); }

        /**
         * @brief Stops collider from being ticked afer this frame.
         * @param Collider Collider to be unregistered.
         */
        inline void UnregisterCollider(Collider* Collider) { Dead.push_back(Collider); }

        /**
         * @brief Stops collider from being ticked. Should not be used inside update loop.
         * @param Collider Collider to be unregistered.
         */
        inline void UnregisterColliderImmediate(Collider* Collider) { std::erase(Updateables, Collider); }

        /**
         * @brief Calls Update() on all updateable Colliders.
         * @param DeltaTime Time since last frame.
         */
        void Update(float DeltaTime);
    };
} // namespace Engine
