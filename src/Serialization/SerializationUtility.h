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
    rapidjson::Value Serialize(const glm::vec3& Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const glm::vec2& Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const Engine::Texture& Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const std::string& Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const GUID& Guid, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const SerializedObject* Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const std::vector<SerializedObject*>& Value,
                               rapidjson::Document::AllocatorType& Allocator);

    template<class T>
    rapidjson::Value Serialize(const std::vector<T*>& Value,
                               rapidjson::Document::AllocatorType& Allocator)
    {
        static_assert(std::is_base_of_v<SerializedObject, T>);

        rapidjson::Value array(rapidjson::kArrayType);
        array.Reserve(static_cast<rapidjson::SizeType>(Value.size()), Allocator);
        for (const T* const object : Value)
        {
            array.PushBack(Serialize(object, Allocator), Allocator);
        }
        return array;
    }


    void Deserialize(const rapidjson::Value& Object, const char* const Name, glm::vec3& Value);

    void Deserialize(const rapidjson::Value& Object, const char* const Name, glm::vec2& Value);

    void Deserialize(const rapidjson::Value& Object, const char* const Name, Engine::Texture& Value);

    void Deserialize(const rapidjson::Value& Object, const char* const Name, std::string& Value);

    void Deserialize(const rapidjson::Value& Object, const char* const Name, GUID& Value);

    void Deserialize(const rapidjson::Value& Object, const char* const Name, SerializedObject*& Value,
                     std::unordered_map<GUID, SerializedObject*, GuidHasher>& ReferenceMap);

    void Deserialize(const rapidjson::Value& Object, SerializedObject*& Value,
                     std::unordered_map<GUID, SerializedObject*, GuidHasher>& ReferenceMap);

    template<class T>
    void Deserialize(const rapidjson::Value& Object, const char* const Name, T*& Value,
                     std::unordered_map<GUID, SerializedObject*, GuidHasher>& ReferenceMap)
    {
        static_assert(std::is_base_of_v<SerializedObject, T>);

        SerializedObject* result;
        Deserialize(Object, Name, result, ReferenceMap);
        Value = reinterpret_cast<T*>(result);
    }

    template<class T>
    void Deserialize(const rapidjson::Value& Object, const char* const Name, const std::vector<T*>& Values,
                     std::unordered_map<GUID, SerializedObject*, GuidHasher>& ReferenceMap)
    {
        static_assert(std::is_base_of_v<SerializedObject, T>);

        for (const rapidjson::Value& value : Object)
        {
            SerializedObject* result;
            Deserialize(value, result, ReferenceMap);
            Values.push_back(reinterpret_cast<T*>(result));
        }
    }


} // Serialization
