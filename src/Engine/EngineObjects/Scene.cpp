//
// Created by Kacper on 17.11.2024.
//

#include "Scene.h"

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
        rapidjson::Value root = rapidjson::Value(rapidjson::kObjectType);
        root.SetObject();
        rapidjson::Value value = rapidjson::Value(rapidjson::kArrayType);
        SerializeEntity(Root, value, Allocator);
        root.AddMember("Objects", value, Allocator);
        return root;
    }

    void Scene::Deserialize(const rapidjson::Value& Value)
    {
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
