#pragma once

#include "Engine/EngineObjects/Entity.h"
#include "Engine/EngineObjects/LightManager.h"
#include "Engine/Textures/Texture.h"
#include <string>

#include "Engine/EngineObjects/GameMode/GameMode.h"
#include "Engine/EngineObjects/Player/Player.h"
#include "Engine/UI/Ui.h"
#include "Models/AabBox.h"

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
    namespace Ui
    {
        class Ui;
    }

    /**
     * @brief Class representing scene hierarchy.
     */
    class Scene final
    {
    private:
        Entity* Root = nullptr;
        GameMode* GameMode = nullptr;
        Player* Player = nullptr;
        Ui::Ui* Ui = nullptr;
        Texture Skybox = Texture();
        std::string Path = "";

        Models::AABBox3 Bounds;

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
         * @brief Returns GameMode used in this scene.
         */
        [[nodiscard]] Engine::GameMode* GetGameMode() const
        {
            return GameMode;
        }

        /**
         * @brief Returns Player used in this scene.
         */
        [[nodiscard]] Engine::Player* GetPlayer() const
        {
            return Player;
        }

        /**
         * @brief Returns UI used in this scene.
         */
        [[nodiscard]] Ui::Ui* GetUi() const
        {
            return Ui;
        }


#if EDITOR
        /**
         * @brief Sets a game mode used in this scene.
         * @param GameMode A new GameMode.
         */
        void SetGameMode(Engine::GameMode* const GameMode)
        {
            delete this->GameMode;
            this->GameMode = GameMode;
            this->GameMode->SetScene(this);
        }

        /**
         * @brief Sets player used in this scene.
         * @param Player A new Player.
         */
        void SetPlayer(Engine::Player* const Player)
        {
            delete this->Player;
            this->Player = Player;
            this->Player->SetScene(this);
            this->Player->GetTransform()->SetParent(Root->GetTransform());
        }

        /**
        * @brief Sets UI used in this scene.
        * @param Ui A new UI.
        */
        void SetUi(Ui::Ui* const Ui)
        {
            delete this->Ui;
            this->Ui = Ui;
        }

        /**
         * @brief Returns world space bounding box for scene geometry.
         */
        [[nodiscard]] Models::AABBox3 GetBounds() const
        {
            return Bounds;
        }
#endif
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
        rapidjson::Value Serialize(rapidjson::Document::AllocatorType& Allocator);

        /**
         * @brief Loads this scene from a json.
         * @param Value Serialized scene.
         */
        void Deserialize(const rapidjson::Value& Value);

        std::string GetPath() const { return Path; }
        void SetPath(const std::string& Path) { this->Path = Path; }

        void DeleteEntity(Entity* Entity);

    private:
        void CalculateBounds();

    private:
        static void SerializeEntity(const Entity* Entity, rapidjson::Value& Object,
                                    rapidjson::Document::AllocatorType& Allocator);

        void CalculateBounds(Entity* Entity);
    };
}
