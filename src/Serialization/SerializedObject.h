#pragma once

#include <combaseapi.h>
#include <unordered_map>

#include "rapidjson/document.h"

namespace Serialization
{

    class SerializedObject
    {
        GUID Id;

    public:
        SerializedObject() // NOLINT(*-pro-type-member-init)
        {
            (void) CoCreateGuid(&Id); //ID Initialized here.
        }

    public:
        [[nodiscard]] GUID GetID() const
        {
            return Id;
        }

    public:
       /* virtual rapidjson::Value Serialize(rapidjson::Document::AllocatorType& Allocator) const = 0;

        virtual void DeserializeValuePass(const rapidjson::Value& Object) = 0;

        virtual void DeserializeReferencesPass(const rapidjson::Value& Object,
                                               std::unordered_map<GUID, SerializedObject*>) = 0;*/
    };

} // Serialization
