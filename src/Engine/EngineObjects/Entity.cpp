#include "Entity.h"

#include "Serialization/SerializationUtility.h"


namespace Engine
{
    Entity::~Entity()
    {
        for (const Component* component : Components)
        {
            delete component;
        }
    }

    rapidjson::Value Entity::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        rapidjson::Value object(rapidjson::kObjectType);
        object.AddMember("type", Serialization::Serialize(TypeName, Allocator), Allocator);
        object.AddMember("id", Serialization::Serialize(GetID(), Allocator), Allocator);
        object.AddMember("transform", Transform.Serialize(Allocator), Allocator);
        object.AddMember("components", Serialization::Serialize(Components, Allocator), Allocator);
        return object;
    }

    void Entity::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        GUID id;
        Serialization::Deserialize(Object, "id", id);
        SetId(id);
        if (const auto transformIterator = Object.FindMember("transform");
            transformIterator != Object.MemberEnd() && transformIterator->value.IsObject())
        {
            Transform.DeserializeValuePass(transformIterator->value, ReferenceMap);
        }
        ReferenceMap.emplace(id, this);
    }

    void Entity::DeserializeReferencesPass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        Transform.DeserializeReferencesPass(Object["transform"], ReferenceMap);
        Serialization::Deserialize(Object, "components", Components, ReferenceMap);
    }
}
