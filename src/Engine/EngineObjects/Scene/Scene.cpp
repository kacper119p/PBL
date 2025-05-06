#include "Scene.h"

#include "Engine/UI/Ui.h"
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

    rapidjson::Value Scene::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        rapidjson::Value documentRoot = rapidjson::Value(rapidjson::kObjectType);
        documentRoot.SetObject();
        rapidjson::Value root = Root->Serialize(Allocator);
        documentRoot.AddMember("Skybox", Serialization::Serialize(Skybox, Allocator), Allocator);
        documentRoot.AddMember("UI", Serialization::Serialize(Ui->GetType(), Allocator), Allocator);
        documentRoot.AddMember("Root", root, Allocator);
        rapidjson::Value objects = rapidjson::Value(rapidjson::kArrayType);
        for (const Component* component : *Root)
        {
            objects.PushBack(component->Serialize(Allocator), Allocator);
        }
        for (const Transform* transform : *(Root->GetTransform()))
        {
            SerializeEntity(transform->GetOwner(), objects, Allocator);
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

        Serialization::Deserialize(Value, "Skybox", Skybox);
        LightManager::GetInstance()->SetEnvironmentMap(Skybox);

        Ui = Ui::UiSerializationFactory::CreateObject(Value["UI"].GetString());

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

        for (const DeserializationPair pair : objects)
        {
            if (Component* component = dynamic_cast<Component*>(pair.Object))
            {
                component->Start();
            }
        }
    }

    void Scene::DeleteEntity(Entity* entity) 
    {
        if (!entity || entity == Root)
            return;

        // Recursively delete children
        std::vector<Transform*> children = entity->GetTransform()->GetChildren();
        for (Transform* child : children)
        {
            Entity* childEntity = child->GetOwner();
            DeleteEntity(childEntity); // Recursive call
        }

        // Remove from parent
        Transform* parentTransform = entity->GetTransform()->GetParent();
        if (parentTransform)
        {
            parentTransform->RemoveChild(entity->GetTransform());
        }

        // Now delete the entity itself
        delete entity;
    }

    void Scene::SerializeEntity(const Entity* const Entity, rapidjson::Value& Object,
                                rapidjson::Document::AllocatorType& Allocator)
    {
        Object.PushBack(Entity->Serialize(Allocator), Allocator);
        for (const Component* component : *Entity)
        {
            Object.PushBack(component->Serialize(Allocator), Allocator);
        }
        for (const Transform* transform : *(Entity->GetTransform()))
        {
            SerializeEntity(transform->GetOwner(), Object, Allocator);
        }
    }
} // Engine
