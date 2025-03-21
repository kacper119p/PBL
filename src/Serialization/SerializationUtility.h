#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <combaseapi.h>
#include <cstring>


#include "Engine/Textures/Texture.h"
#include "rapidjson/document.h"
#include "SerializedObject.h"

namespace Serialization
{
    rapidjson::Value Serialize(const glm::vec3& Value, const rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const glm::vec2& Value, const rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const Engine::Texture& Value, const rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const GUID& Guid, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const SerializedObject* const Value, rapidjson::Document::AllocatorType& Allocator);

    void Deserialize(const rapidjson::Value& Object, glm::vec3& Value);

    void Deserialize(const rapidjson::Value& Object, glm::vec2& Value);

    void Deserialize(const rapidjson::Value& Object, Engine::Texture& Value);

    void Deserialize(const rapidjson::Value& Object, GUID& Value);

    void Deserialize(const rapidjson::Value& Object, SerializedObject*& Value,
                     std::unordered_map<GUID, SerializedObject*>& ReferenceMap);

    template<class T>
    void Deserialize(const rapidjson::Value& Object, T*& Value,
                     std::unordered_map<GUID, SerializedObject*>& ReferenceMap)
    {
        SerializedObject* result;
        Deserialize(Object, result, ReferenceMap);
        Value = static_cast<T*>(result);
    }
} // Serialization

inline bool operator < (const GUID &guid1, const GUID &guid2) {
    if(guid1.Data1!=guid2.Data1) {
        return guid1.Data1 < guid2.Data1;
    }
    if(guid1.Data2!=guid2.Data2) {
        return guid1.Data2 < guid2.Data2;
    }
    if(guid1.Data3!=guid2.Data3) {
        return guid1.Data3 < guid2.Data3;
    }
    for(int i=0;i<8;i++) {
        if(guid1.Data4[i]!=guid2.Data4[i]) {
            return guid1.Data4[i] < guid2.Data4[i];
        }
    }
    return false;
}

