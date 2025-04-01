#pragma once

#include "Engine/EngineObjects/Entity.h"
#include "Engine/EngineObjects/LightManager.h"
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
        Texture Skybox;

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

        [[nodiscard]] Texture GetSkybox() const
        {
            return Skybox;
        }

        void SetSkybox(const Texture& Skybox)
        {
            this->Skybox = Skybox;
            LightManager::GetInstance()->SetEnvironmentMap(Skybox);
        }

        Entity* SpawnEntity(Entity* Parent);

        Entity* SpawnEntity(Entity* Parent, const glm::vec3& Position, const glm::vec3& Rotation);


        rapidjson::Value Serialize(rapidjson::Document::AllocatorType& Allocator) const;

        void Deserialize(const rapidjson::Value& Value);

    private:
        static void SerializeEntity(const Entity* Entity, rapidjson::Value& Object,
                                    rapidjson::Document::AllocatorType& Allocator);
    };
}
