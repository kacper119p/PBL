#pragma once

#include <combaseapi.h>
#include <unordered_map>

#include "GuidHasher.h"
#include "rapidjson/document.h"

#define START_OBJECT_SERIALIZATION rapidjson::Value object(rapidjson::kObjectType);\
                                   object.AddMember("type", Serialization::Serialize(typeid(this).name(), Allocator), Allocator);\
                                   object.AddMember("id", Serialization::Serialize(GetID(), Allocator), Allocator);\
                                   object.AddMember("owner", Serialization::Serialize(GetOwner(), Allocator), Allocator);

#define END_OBJECT_SERIALIZATION  return object;

#define START_OBJECT_DESERIALIZATION_VALUE_PASS GUID id;\
                                                Serialization::Deserialize(Object["id"], id);\
                                                SetId(id);

#define END_OBJECT_DESERIALIZATION_VALUE_PASS   ReferenceMap.emplace(id, this);

#define START_OBJECT_DESERIALIZATION_REFERENCES_PASS Entity* owner;\
                                                     Serialization::Deserialize(Object["owner"], owner, ReferenceMap);\
                                                     SetOwner(owner);

#define END_OBJECT_DESERIALIZATION_REFERENCES_PASS

namespace Serialization
{
    class SerializedObject;
    typedef std::unordered_map<GUID, SerializedObject*, GuidHasher> ReferenceTable;

    class SerializedObject
    {
    private:
        GUID Id;

    public:
        virtual ~SerializedObject() = default;

        SerializedObject() // NOLINT(*-pro-type-member-init)
        {
            (void) CoCreateGuid(&Id); //ID Initialized here.
        }

    public:
        [[nodiscard]] GUID GetID() const
        {
            return Id;
        }

    protected:
        void SetId(const GUID& Id)
        {
            this->Id = Id;
        }

    public:
        virtual rapidjson::Value Serialize(rapidjson::Document::AllocatorType& Allocator) const = 0;

        virtual void DeserializeValuePass(const rapidjson::Value& Object, ReferenceTable& ReferenceMap) = 0;

        virtual void DeserializeReferencesPass(const rapidjson::Value& Object, ReferenceTable& ReferenceMap) =0;
    };

} // Serialization
