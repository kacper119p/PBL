#pragma once

#include "Entity.h"
#include "Engine/Textures/Texture.h"
#include "Shaders/Shader.h"

namespace Materials
{
    class Material;
}

namespace Models
{
    class Model;
}

namespace Engine
{
    /**
     * @brief Class representing scene hierarchy.
     */
    class Scene final
    {
    private:
        Entity* Root;

    public:
        std::vector<Texture> Textures;
        std::vector<Models::Model*> Models;
        std::vector<Shaders::Shader> Shaders;
        std::vector<Materials::Material*> Materials;

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

    public:
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
