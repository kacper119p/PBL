#include "Entity.h"

#include "GizmoManager.h"
#include "Scene/Scene.h"
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
    std::vector<Entity*> Entity::ToDestroy;

    Entity::~Entity()
    {
        for (Component* component : Components)
        {
            component->OnDestroy();
            delete component;
        }
#if EDITOR
        if (GizmoManager::GetInstance()->GetManaged() == this->GetTransform())
        {
            GizmoManager::GetInstance()->SetManaged(nullptr);
        }
#endif
    }

    Entity* Entity::CloneAsConcrete() const
    {
        rapidjson::Document document;
        document.SetArray();
        SerializeEntity(document, document.GetAllocator());
        return DeserializeEntity(document, GetScene(), GetTransform()->GetParent());
    }

    void Entity::SerializeEntity(rapidjson::Value& Object, rapidjson::Document::AllocatorType& Allocator) const
    {
        {
            Object.PushBack(this->Serialize(Allocator), Allocator);
            for (const Component* component : *this)
            {
                Object.PushBack(component->Serialize(Allocator), Allocator);
            }
            for (const class Transform* transform : *(this->GetTransform()))
            {
                transform->GetOwner()->SerializeEntity(Object, Allocator);
            }
        }
    }

    Entity* Entity::DeserializeEntity(rapidjson::Value& Object, class Scene* Scene, class Transform* Parent)
    {
        Serialization::ReferenceTable referenceTable;
        std::vector<DeserializationPair> objects;
        Entity* root = nullptr;

        for (const rapidjson::Value& jsonObject : Object.GetArray())
        {
            SerializedObject* deserializedObject = Serialization::SerializedObjectFactory::CreateObject(
                    jsonObject["type"].GetString());
            deserializedObject->DeserializeValuePass(jsonObject, referenceTable);
            DeserializationPair pair{jsonObject, deserializedObject};
            objects.emplace_back(pair);
            if (Entity* entity = dynamic_cast<Entity*>(deserializedObject))
            {
                entity->Scene = Scene;
                if (root == nullptr)
                {
                    root = entity;
                }
            }
        }

        for (DeserializationPair pair : objects)
        {
            pair.Object->DeserializeReferencesPass(pair.Json, referenceTable);
        }

        for (const DeserializationPair pair : objects)
        {
            pair.Object->SetId(Utility::GenerateGuid());
        }

        if (root->GetTransform()->GetParent() == nullptr)
        {
            if (Parent == nullptr)
            {
                root->GetTransform()->SetParent(Scene->GetRoot()->GetTransform());
            }
            else
            {
                root->GetTransform()->SetParent(Parent);
            }
        }

        for (const DeserializationPair pair : objects)
        {
            if (Component* component = dynamic_cast<Component*>(pair.Object))
            {
                component->Start();
            }
        }

        return root;
    }

#if EDITOR
    void Entity::DrawImGui()
    {
        Transform.DrawImGui();
        for (size_t i = 0; i < Components.size();)
        {
            Component* component = Components[i];
            component->DrawImGui();

            ImGui::PushID(static_cast<int>(i));
            if (ImGui::Button("Remove"))
            {
                this->RemoveComponent(component);
                ImGui::PopID();
                continue;
            }
            ImGui::PopID();
            ++i;
        }
    }
#endif

    rapidjson::Value Entity::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        rapidjson::Value object(rapidjson::kObjectType);
        object.AddMember("type", Serialization::Serialize(TypeName, Allocator), Allocator);
        object.AddMember("id", Serialization::Serialize(GetID(), Allocator), Allocator);
        object.AddMember("name", Serialization::Serialize(GetName(), Allocator), Allocator);
        object.AddMember("transform", Transform.Serialize(Allocator), Allocator);
        object.AddMember("components", Serialization::Serialize(Components, Allocator), Allocator);
        return object;
    }

    void Entity::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        GUID id;
        Serialization::Deserialize(Object, "id", id);
        SetId(id);
        Serialization::Deserialize(Object, "name", Name);
        if (const auto transformIterator = Object.FindMember("transform");
            transformIterator != Object.MemberEnd() && transformIterator->value.IsObject())
        {
            Transform.DeserializeValuePass(transformIterator->value, ReferenceMap);
        }
        ReferenceMap.emplace(id, this);
    }

    void Entity::DeserializeReferencesPass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        if (const auto transformIterator = Object.FindMember("transform");
            transformIterator != Object.MemberEnd() && transformIterator->value.IsObject())
        {
            Transform.DeserializeReferencesPass(transformIterator->value, ReferenceMap);
        }
        Serialization::Deserialize(Object, "components", Components, ReferenceMap);
    }
}
