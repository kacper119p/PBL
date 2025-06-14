#pragma once
#include <glm/vec3.hpp>
#include <string>

#include "GuidHasher.h"
#include "rapidjson/document.h"
#include "Materials/Material.h"
#include "Materials/Properties/MaterialProperty.h"
#include "Materials/Properties/TextureMaterialProperty.h"
#include "Models/AabBox.h"
#include "Shaders/ComputeShader.h"
#include "Shaders/Shader.h"

namespace Engine
{
    class Texture;
}

namespace Models
{
    class Model;
    class ModelAnimated;
    class Animation;
    class Animator;
}

namespace Serialization
{
    class SerializedObject;
}


namespace Serialization
{
    rapidjson::Value Serialize(int Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(float Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(bool Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const glm::quat& Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const glm::vec4& Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const glm::vec3& Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const glm::vec2& Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const Engine::Texture& Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const Models::Model* Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const Models::ModelAnimated* Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const Models::Animation* Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const std::string& Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const GUID& Guid, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const SerializedObject* Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const std::vector<SerializedObject*>& Value,
                               rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(Shaders::Shader Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(Shaders::ComputeShader Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const Materials::Material* Value, rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const Materials::TextureMaterialProperty& Value,
                               rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const Models::AABBox3 Value, rapidjson::Document::AllocatorType& Allocator);

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

    template<typename T>
    rapidjson::Value Serialize(const Materials::MaterialProperty<T>& Value,
                               rapidjson::Document::AllocatorType& Allocator)
    {
        T value = Value.GetValue();
        return Serialize(value, Allocator);
    }

    template<typename E>
        requires std::is_enum_v<E>
    rapidjson::Value Serialize(const E Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kNumberType);
        object.SetInt(Value);
        return object;
    }

    void Deserialize(const rapidjson::Value& Object, const char* Name, int& Value);

    void Deserialize(const rapidjson::Value& Object, const char* Name, float& Value);

    void Deserialize(const rapidjson::Value& Object, const char* Name, bool& Value);

    void Deserialize(const rapidjson::Value& Object, const char* Name, glm::quat& Value);

    void Deserialize(const rapidjson::Value& Object, const char* Name, glm::vec4& Value);

    void Deserialize(const rapidjson::Value& Object, const char* Name, glm::vec3& Value);

    void Deserialize(const rapidjson::Value& Object, const char* Name, glm::vec2& Value);

    void Deserialize(const rapidjson::Value& Object, const char* Name, Engine::Texture& Value);

    void Deserialize(const rapidjson::Value& Object, const char* Name, Models::Model*& Value);

    void Deserialize(const rapidjson::Value& Object, const char* Name, Models::ModelAnimated*& Value);

    void Deserialize(const rapidjson::Value& Object, const char* Name, Models::Animation*& Value);

    void Deserialize(const rapidjson::Value& Object, const char* Name, std::string& Value);

    void Deserialize(const rapidjson::Value& Object, const char* Name, GUID& Value);

    void Deserialize(const rapidjson::Value& Object, const char* Name, SerializedObject*& Value,
                     std::unordered_map<GUID, SerializedObject*, GuidHasher>& ReferenceMap);

    void Deserialize(const rapidjson::Value& Object, SerializedObject*& Value,
                     std::unordered_map<GUID, SerializedObject*, GuidHasher>& ReferenceMap);

    void Deserialize(const rapidjson::Value& Object, const char* Name, Shaders::Shader& Value);

    void Deserialize(const rapidjson::Value& Object, const char* Name, Shaders::ComputeShader& Value);

    void Deserialize(const rapidjson::Value& Object, const char* Name, Materials::Material*& Value);

    void Deserialize(const rapidjson::Value& Object, const char* Name, Materials::TextureMaterialProperty& Value);

    void Deserialize(const rapidjson::Value& Object, const char* Name, Models::AABBox3& Value);

    template<class T>
    void Deserialize(const rapidjson::Value& Object, const char* const Name, T*& Value,
                     std::unordered_map<GUID, SerializedObject*, GuidHasher>& ReferenceMap)
    {
        // static_assert(std::is_base_of_v<SerializedObject, T>);

        SerializedObject* result;
        Deserialize(Object, Name, result, ReferenceMap);
        Value = reinterpret_cast<T*>(result);
    }

    template<class T>
    void Deserialize(const rapidjson::Value& Object, const char* const Name, std::vector<T*>& Values,
                     std::unordered_map<GUID, SerializedObject*, GuidHasher>& ReferenceMap)
    {
        static_assert(std::is_base_of_v<SerializedObject, T>);

        const auto memberIterator = Object.FindMember(Name);
        if (memberIterator == Object.MemberEnd() || !memberIterator->value.IsArray())
        {
            return;
        }

        for (const rapidjson::Value& value : memberIterator->value.GetArray())
        {
            SerializedObject* result;
            Deserialize(value, result, ReferenceMap);
            if (result != nullptr)
            {
                Values.push_back(reinterpret_cast<T*>(result));
            }
        }
    }

    template<typename T>
    void Deserialize(const rapidjson::Value& Object, const char* const Name, Materials::MaterialProperty<T>& Value)
    {
        T value = Value.GetValue();
        Deserialize(Object, Name, value);
        Value.SetValue(value);
    }

    template<typename E>
        requires std::is_enum_v<E>
    void Deserialize(
            const rapidjson::Value& Object, const char* Name, E& Value)
    {
        const auto iterator = Object.FindMember(Name);
        if (iterator == Object.MemberEnd() || !iterator->value.IsInt())
        {
            return;
        }
        Value = static_cast<E>(iterator->value.GetInt());
    }
}
