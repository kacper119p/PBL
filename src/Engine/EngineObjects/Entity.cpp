#include "Entity.h"

#include "GizmoManager.h"
#include "Serialization/SerializationUtility.h"


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
