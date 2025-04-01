#pragma once
#pragma comment(lib, "Ole32.lib")
#include <windows.h>
#include <combaseapi.h>
#include <string>
#include <unordered_map>

#include "GuidHasher.h"
#include "Utility/AssertionsUtility.h"
#include "rapidjson/document.h"

#define SERIALIZATION_EXPORT_CLASS(__CLASS__)\
public:\
    inline static const std::string TypeName = std::string(#__CLASS__);\
    rapidjson::Value Serialize(rapidjson::Document::AllocatorType& Allocator) const override;\
    void DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap) override;\
    void DeserializeReferencesPass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap) override;\
    [[nodiscard]] std::string GetType() const override\
    {\
        return TypeName;\
    }

#define START_COMPONENT_SERIALIZATION\
    rapidjson::Value object(rapidjson::kObjectType);\
    object.AddMember("type", Serialization::Serialize(TypeName, Allocator), Allocator);\
    object.AddMember("id", Serialization::Serialize(GetID(), Allocator), Allocator);\
    object.AddMember("owner", Serialization::Serialize(GetOwner(), Allocator), Allocator);

#define END_COMPONENT_SERIALIZATION  return object;

#define START_COMPONENT_DESERIALIZATION_VALUE_PASS\
    GUID id;\
    Serialization::Deserialize(Object, "id", id);\
    SetId(id);

#define END_COMPONENT_DESERIALIZATION_VALUE_PASS   ReferenceMap.emplace(id, this);

#define START_COMPONENT_DESERIALIZATION_REFERENCES_PASS\
    Entity* owner;\
    Serialization::Deserialize(Object, "owner", owner, ReferenceMap);\
    SetOwner(owner);

#define END_COMPONENT_DESERIALIZATION_REFERENCES_PASS

#define SERIALIZE_FIELD(__NAME__) object.AddMember(#__NAME__, Serialization::Serialize(__NAME__, Allocator), Allocator);

#define DESERIALIZE_VALUE(__NAME__) Serialization::Deserialize(Object, #__NAME__, __NAME__);

#define DESERIALIZE_POINTER(__NAME__) Serialization::Deserialize(Object, #__NAME__, (__NAME__), ReferenceMap);

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
            const HRESULT result = CoCreateGuid(&Id); //ID Initialized here.
            CHECK_MESSAGE(result == S_OK, "Failed to initialize GUID")
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

        /**
        * @brief Returns class name of this object.
        */
        [[nodiscard]] virtual std::string GetType() const = 0;
    };

} // Serialization
