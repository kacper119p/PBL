#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <rpc.h>
#include <string>


#include "GuidHasher.h"
#include "Engine/Textures/Texture.h"
#include "rapidjson/document.h"
#include "SerializedObject.h"

namespace Serialization
{
    rapidjson::Value Serialize(const glm::vec3& Value, const rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const glm::vec2& Value, const rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const Engine::Texture& Value, const rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const std::string& Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const GUID& Guid, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const SerializedObject* Value, rapidjson::Document::AllocatorType& Allocator);

    void Deserialize(const rapidjson::Value& Object, glm::vec3& Value);

    void Deserialize(const rapidjson::Value& Object, glm::vec2& Value);

    void Deserialize(const rapidjson::Value& Object, Engine::Texture& Value);

    void Deserialize(const rapidjson::Value& Object, std::string& Value);

    void Deserialize(const rapidjson::Value& Object, GUID& Value);

    void Deserialize(const rapidjson::Value& Object, SerializedObject*& Value,
                     std::unordered_map<GUID, SerializedObject*, GuidHasher>& ReferenceMap);

    template<class T>
    void Deserialize(const rapidjson::Value& Object, T*& Value,
                     std::unordered_map<GUID, SerializedObject*, GuidHasher>& ReferenceMap)
    {
        SerializedObject* result;
        Deserialize(Object, result, ReferenceMap);
        Value = static_cast<T*>(result);
    }
} // Serialization
