#pragma once

#include "Entity.h"

namespace Engine
{
    /**
     * @brief Class representing scene hierarchy.
     */
    class Scene
    {
    private:
        Entity* Root;

    public:
        /**
         * @brief Constructs a new scene.
         * @param Root Entity to be used as a scene;s root.
         */
        explicit Scene(Entity* Root);

        /**
         * @brief Constructs a new scene with an empty entity as its root.
         */
        Scene();

        virtual ~Scene();

    public:
        /**
         * @brief Returns root of this scene.
         */
        [[nodiscard]] Entity* GetRoot() const
        {
            return Root;
        }

        /**
         * @brief Sets root of this scene.
         * @param Root A new root.
         */
        void SetRoot(Entity* Root)
        {
            this->Root = Root;
        }
    };
}
