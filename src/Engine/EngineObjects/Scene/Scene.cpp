#include "Scene.h"

#include "Engine/Components/Renderers/AnimatedModelRenderer.h"
#include "Engine/Components/Renderers/ModelRenderer.h"
#include "Engine/EngineObjects/GameMode/DefaultGameMode.h"
#include "Engine/EngineObjects/Player/DefaultPlayer.h"
#include "Engine/UI/Ui.h"
#include "Engine/UI/UiImplementations/EmptyUi.h"
#include "Serialization/SerializedObjectFactory.h"
#include "Serialization/SerializationUtility.h"

namespace
{
    struct DeserializationPair
    {
        const rapidjson::Value& Json;
        Serialization::SerializedObject* Object;
    };
}

namespace Engine
{
    Scene::Scene()
    {
        Root = new Entity();
        Root->SetName("Root");
    }

    Scene::Scene(Entity* Root) :
        Root(Root)
    {
    }

    Scene::~Scene()
    {
        delete Root;
        delete Ui;
        delete GameMode;
        delete Player;
    }

    Entity* Scene::SpawnEntity(Entity* const Parent)
    {
        Entity* result = new Entity();
        if (Parent != nullptr)
        {
#if DEBUG
            //CHECK_MESSAGE(Parent->GetScene() == this, "Parent doesn't belong to this scene.")
#endif
            result->GetTransform()->SetParent(Parent->GetTransform());
        }
        else
        {
            result->GetTransform()->SetParent(Root->GetTransform());
        }
        return result;
    }

    Entity* Scene::SpawnEntity(Entity* const Parent, const glm::vec3& Position, const glm::vec3& Rotation)
    {
#if DEBUG
        CHECK_MESSAGE(Parent->GetScene() == this, "Parent doesn't belong to this scene.")
#endif
        Entity* result = new Entity();
        result->GetTransform()->SetParent(Parent->GetTransform());
        result->GetTransform()->SetPosition(Position);
        result->GetTransform()->SetEulerAngles(Rotation);
        return result;
    }

    rapidjson::Value Scene::Serialize(rapidjson::Document::AllocatorType& Allocator)
    {
        CalculateBounds();
        rapidjson::Value documentRoot = rapidjson::Value(rapidjson::kObjectType);
        documentRoot.SetObject();
        rapidjson::Value root = Root->Serialize(Allocator);
        documentRoot.AddMember("Skybox", Serialization::Serialize(Skybox, Allocator), Allocator);
        documentRoot.AddMember("Bounds", Serialization::Serialize(Bounds, Allocator), Allocator);
        documentRoot.AddMember("UI", Serialization::Serialize(Ui->GetType(), Allocator), Allocator);
        documentRoot.AddMember("GameMode", Serialization::Serialize(GameMode->GetType(), Allocator), Allocator);
        documentRoot.AddMember("Player", Serialization::Serialize(Player->GetType(), Allocator), Allocator);
        documentRoot.AddMember("Root", root, Allocator);
        rapidjson::Value objects = rapidjson::Value(rapidjson::kArrayType);
        for (const Component* component : *Root)
        {
            objects.PushBack(component->Serialize(Allocator), Allocator);
        }
        for (const Transform* transform : *(Root->GetTransform()))
        {
            transform->GetOwner()->SerializeEntity(objects, Allocator);
        }

        documentRoot.AddMember("Objects", objects, Allocator);
        return documentRoot;
    }

    void Scene::Deserialize(const rapidjson::Value& Value)
    {
        Serialization::ReferenceTable referenceTable;
        std::vector<DeserializationPair> objects;

        delete Root;
        Root = new Entity();
        Root->SetName("Root");

        delete Ui;
        delete GameMode;
        delete Player;

        Serialization::Deserialize(Value, "Skybox", Skybox);
        LightManager::GetInstance()->SetEnvironmentMap(Skybox);

        Serialization::Deserialize(Value, "Bounds", Bounds);

        const auto uiIterator = Value.FindMember("UI");
        if (uiIterator != Value.MemberEnd())
        {
            Ui = Ui::UiSerializationFactory::CreateObject(uiIterator->value.GetString());
        }
        else
        {
            Ui = new Ui::EmptyUi();
        }

        const auto gameModeIterator = Value.FindMember("GameMode");
        if (gameModeIterator != Value.MemberEnd())
        {
            GameMode = GameModeFactory::CreateObject(gameModeIterator->value.GetString());
        }
        else
        {
            GameMode = new DefaultGameMode();
        }

        const auto playerIterator = Value.FindMember("Player");
        if (playerIterator != Value.MemberEnd())
        {
            Player = PlayerFactory::CreateObject(playerIterator->value.GetString());
        }
        else
        {
            Player = new DefaultPlayer();
        }

        Root->Scene = this;
        Player->Scene = this;
        GameMode->Scene = this;

        Root->DeserializeValuePass(Value["Root"], referenceTable);

        const rapidjson::Value& serializedObjects = Value["Objects"];

        for (const rapidjson::Value& jsonObject : serializedObjects.GetArray())
        {
            Serialization::SerializedObject* deserializedObject
                    = Serialization::SerializedObjectFactory::CreateObject(jsonObject["type"].GetString());
            deserializedObject->DeserializeValuePass(jsonObject, referenceTable);
            DeserializationPair pair{jsonObject, deserializedObject};
            objects.emplace_back(pair);
            if (Entity* entity = dynamic_cast<Entity*>(deserializedObject))
            {
                entity->Scene = this;
            }
        }

        Root->DeserializeReferencesPass(Value["Root"], referenceTable);
        for (DeserializationPair pair : objects)
        {
            pair.Object->DeserializeReferencesPass(pair.Json, referenceTable);
        }

        GameMode->Start();
        Player->Start();

        for (const DeserializationPair pair : objects)
        {
            if (Component* component = dynamic_cast<Component*>(pair.Object))
            {
                component->Start();
            }
        }
    }

    void Scene::DeleteEntity(Entity* const Entity)
    {
        if (!Entity || Entity == Root)
            return;

        delete Entity;
    }

    void Scene::CalculateBounds()
    {
        Bounds = Models::AABBox3(glm::vec3(0.0f), glm::vec3(0.0f));
        CalculateBounds(Root);
    }

    void Scene::CalculateBounds(Entity* const Entity)
    {
        for (const Transform* child : *Entity->GetTransform())
        {
            CalculateBounds(child->GetOwner());
        }
        if (const ModelRenderer* renderer = Entity->GetComponent<ModelRenderer>())
        {
            const Models::Model* model = renderer->GetModel();
            const glm::mat4& modelMatrix = Entity->GetTransform()->GetLocalToWorldMatrix();
            for (int i = 0; i < model->GetMeshCount(); ++i)
            {
                const Models::Mesh* mesh = model->GetMesh(i);
                Models::AABBox3 aabb = mesh->GetAabBox().ToWorldSpace(modelMatrix);
                Bounds.min = glm::min(Bounds.min, aabb.min);
                Bounds.max = glm::max(Bounds.max, aabb.max);
            }
        }
        else if (const AnimatedModelRenderer* animatedRenderer = Entity->GetComponent<AnimatedModelRenderer>())
        {
            const Models::ModelAnimated* model = animatedRenderer->GetModel();
            const glm::mat4& modelMatrix = Entity->GetTransform()->GetLocalToWorldMatrix();
            for (int i = 0; i < model->GetMeshCount(); ++i)
            {
                const Models::MeshAnimated* mesh = model->GetMesh(i);
                Models::AABBox3 aabb = mesh->GetAabBox().ToWorldSpace(modelMatrix);
                Bounds.min = glm::min(Bounds.min, aabb.min);
                Bounds.max = glm::max(Bounds.max, aabb.max);
            }
        }
    }
}
