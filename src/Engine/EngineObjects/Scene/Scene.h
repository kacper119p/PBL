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
         * @brief Returns this scene's skybox.
         */
        [[nodiscard]] Texture GetSkybox() const
        {
            return Skybox;
        }

        /**
         * @brief Sets a new skybox for this scene.
         * @param Skybox A new skybox.
         */
        void SetSkybox(const Texture& Skybox)
        {
            this->Skybox = Skybox;
            LightManager::GetInstance()->SetEnvironmentMap(Skybox);
        }

        /**
         * @brief Spawns a new Entity in this scene.
         * @param Parent Parent of this object. If nullptr scene root becomes parent.
         * @return Created entity.
         */
        Entity* SpawnEntity(Entity* Parent);

        /**
         * @brief Spawns a new Entity in this scene at a given position and with given rotation.
         * @param Parent Parent of this object. If nullptr scene root becomes parent.
         * @param Position Position of a created object.
         * @param Rotation Rotation of a created object.
         * @return
         */
        Entity* SpawnEntity(Entity* Parent, const glm::vec3& Position, const glm::vec3& Rotation);

        /**
         * @brief Saves this scene to a json.
         * @param Allocator An allocator to be used.
         * @return Serialized scene.
         */
        rapidjson::Value Serialize(rapidjson::Document::AllocatorType& Allocator) const;

        /**
         * @brief Loads this scene from a json.
         * @param Value Serialized scene.
         */
        void Deserialize(const rapidjson::Value& Value);

    private:
        static void SerializeEntity(const Entity* Entity, rapidjson::Value& Object,
                                    rapidjson::Document::AllocatorType& Allocator);
    };
}
