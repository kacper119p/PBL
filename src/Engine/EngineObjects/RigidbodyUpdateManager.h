#pragma once

#include <vector>
#include "Engine/Components/Physics/Rigidbody.h"
namespace Engine
{
     /**
     * @brief Singleton responsible for updating RigidBody components.
     */
    class RigidbodyUpdateManager
    {
    private:
        static RigidbodyUpdateManager* Instance;

        std::vector<RigidBody*> Updateables;
        std::vector<RigidBody*> Dead;

    private:
        RigidbodyUpdateManager();

    public:
        /**
         * @brief Initializes the RigidbodyUpdateManager singleton.
         */
        static void Initialize();

        /**
         * @brief Returns the instance of the RigidbodyUpdateManager.
         */
        static RigidbodyUpdateManager* GetInstance() { return Instance; }

        /**
         * @brief Registers a new RigidBody to be updated.
         * @param Rigidbody The RigidBody to be registered.
         */
        inline void RegisterRigidbody(RigidBody* Rigidbody) { Updateables.push_back(Rigidbody); }

        /**
         * @brief Marks a RigidBody to stop being updated after the current frame.
         * @param Rigidbody The RigidBody to be unregistered.
         */
        inline void UnregisterRigidbody(RigidBody* Rigidbody) { Dead.push_back(Rigidbody); }

        /**
         * @brief Immediately stops updating a RigidBody. Should not be used inside the update loop.
         * @param Rigidbody The RigidBody to be unregistered.
         */
        inline void UnregisterRigidbodyImmediate(RigidBody* Rigidbody) { std::erase(Updateables, Rigidbody); }

        /**
         * @brief Updates all registered RigidBody components.
         * @param DeltaTime The time elapsed since the last frame.
         */
        void Update(float DeltaTime);
    };
} // namespace Engine
