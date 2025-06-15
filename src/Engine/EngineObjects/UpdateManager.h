#pragma once

#include <vector>
#include "Engine/Components/Interfaces/IUpdateable.h"

namespace Engine
{
    class GameMode;
}

namespace Engine
{
    /**
     * @brief Singleton responsible for ticking updateable components.
     */
    class UpdateManager
    {
    private:
        static UpdateManager* Instance;

        std::vector<IUpdateable*> Updateables;
        std::vector<IUpdateable*> Dead;

        GameMode* GameMode = nullptr;

    private:
        UpdateManager();

    public:
        /**
         * @brief Initializes a new UpdateManager.
         */
        static void Initialize();

        /**
         * @brief Returns instance of the UpdateManager.
         */
        static UpdateManager* GetInstance()
        {
            return Instance;
        }

        /**
         * @brief Registers new Updateable to be ticked.
         * @param Component Component to be registered.
         */
        void RegisterComponent(IUpdateable* Component)
        {
            Updateables.push_back(Component);
        }

        /**
         * @brief Stops component from being ticked afer this frame.
         * @param Component Component to be unregistered.
         */
        void UnregisterComponent(IUpdateable* Component)
        {
            Dead.push_back(Component);
        }

        /**
         * @brief Stops component from being ticked. Should not be used inside update loop.
         * @param GameMode GameMode  to be unregistered
         */
        void UnregisterGameMode(const class GameMode* const GameMode)
        {
            if (this->GameMode == GameMode)
            {
                this->GameMode = nullptr;
            }
        }

        /**
         * @brief Registers new GameMode to be ticked.
         * @param GameMode GameMode to be registered.
         */
        void RegisterGameMode(class GameMode* const GameMode)
        {
            this->GameMode = GameMode;
        }

        /**
         * @brief Calls Update() on all Updateables.
         * @param DeltaTime Time since last frame.
         */
        void Update(float DeltaTime);
    };
} // Engine
