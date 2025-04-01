#include "Scene.h"

#include "SerializedObjectFactory.h"

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
    }

    Scene::Scene(Entity* Root) :
        Root(Root)
    {
    }

    Scene::~Scene()
    {
        delete Root;
    }

    rapidjson::Value Scene::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        rapidjson::Value documentRoot = rapidjson::Value(rapidjson::kObjectType);
        documentRoot.SetObject();
        rapidjson::Value root = rapidjson::Value(rapidjson::kObjectType);
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

        Root->DeserializeValuePass(Value["Root"], referenceTable);

        const rapidjson::Value& serializedObjects = Value["Objects"];

        for (const rapidjson::Value& jsonObject : serializedObjects.GetArray())
        {
            Serialization::SerializedObject* deserializedObject = SerializedObjectFactory::CreateObject(jsonObject);
            deserializedObject->DeserializeValuePass(jsonObject, referenceTable);
            DeserializationPair pair{jsonObject, deserializedObject};
            objects.emplace_back(pair);
        }

        Root->DeserializeReferencesPass(Value["Root"], referenceTable);
        for (DeserializationPair pair : objects)
        {
            pair.Object->DeserializeReferencesPass(pair.Json, referenceTable);
        }
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
