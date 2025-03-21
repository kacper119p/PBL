#include "SerializationUtility.h"

#include <atlbase.h>
#include <locale>

#include "Engine/Textures/TextureManager.h"

namespace
{
    constexpr int GUID_STRING_SIZE = 39; // GUID is 36 chars + null terminator

    void GuidToStr(const GUID& guid, char* buffer)
    {
        snprintf(buffer, sizeof(buffer),
                 "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
                 guid.Data1, guid.Data2, guid.Data3,
                 guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
                 guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    }

    void StrToGuid(const std::string& str, GUID* guid)
    {
        sscanf_s(str.c_str(),
                 "{%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx}",
                 &guid->Data1, &guid->Data2, &guid->Data3,
                 &guid->Data4[0], &guid->Data4[1], &guid->Data4[2], &guid->Data4[3],
                 &guid->Data4[4], &guid->Data4[5], &guid->Data4[6], &guid->Data4[7]);
    }
}

namespace Serialization
{
    rapidjson::Value Serialize(const glm::vec3& Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kObjectType);
        object.AddMember("x", Value.x, Allocator);
        object.AddMember("y", Value.y, Allocator);
        object.AddMember("z", Value.z, Allocator);
        return object;
    }

    rapidjson::Value Serialize(const glm::vec2& Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kObjectType);
        object.AddMember("x", Value.x, Allocator);
        object.AddMember("y", Value.y, Allocator);
        return object;
    }

    rapidjson::Value Serialize(const Engine::Texture& Value)
    {
        rapidjson::Value object(rapidjson::kStringType);
        const std::string path = Engine::TextureManager::GetTexturePath(Value);
        object.SetString(path.c_str(), static_cast<rapidjson::SizeType>(path.length()));
        return object;
    }

    rapidjson::Value Serialize(const std::string& Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kStringType);
        object.SetString(Value.c_str(), static_cast<rapidjson::SizeType>(Value.length()), Allocator);
        return object;
    }

    rapidjson::Value Serialize(const GUID& Guid, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kStringType);
        char guidStr[GUID_STRING_SIZE];
        GuidToStr(Guid, guidStr);
        object.SetString(guidStr, static_cast<rapidjson::SizeType>(GUID_STRING_SIZE), Allocator);
        return object;
    }

    rapidjson::Value Serialize(const SerializedObject* const Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kStringType);
        if (Value == nullptr)
        {
            object.SetNull();
            return object;
        }
        const GUID guid = Value->GetID();
        char guidStr[GUID_STRING_SIZE];
        GuidToStr(guid, guidStr);
        object.SetString(guidStr, sizeof(guidStr), Allocator);
        return object;
    }

    rapidjson::Value Serialize(const std::vector<SerializedObject*>& Value,
                               rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value array(rapidjson::kArrayType);
        array.Reserve(static_cast<rapidjson::SizeType>(Value.size()), Allocator);
        for (const SerializedObject* const object : Value)
        {
            array.PushBack(Serialize(object, Allocator), Allocator);
        }
        return array;
    }


    void Deserialize(const rapidjson::Value& Object, glm::vec3& Value)
    {
        Value.x = Object["x"].GetFloat();
        Value.y = Object["y"].GetFloat();
        Value.z = Object["z"].GetFloat();
    }

    void Deserialize(const rapidjson::Value& Object, glm::vec2& Value)
    {
        Value.x = Object["x"].GetFloat();
        Value.y = Object["y"].GetFloat();
    }

    void Deserialize(const rapidjson::Value& Object, Engine::Texture& Value)
    {
        Value = Engine::TextureManager::GetTexture(Object.GetString());
    }

    void Deserialize(const rapidjson::Value& Object, std::string& Value)
    {
        Value = Object.GetString();
    }

    void Deserialize(const rapidjson::Value& Object, GUID& Value)
    {
        const std::string str = Object.GetString();
        StrToGuid(str, &Value);
    }

    void Deserialize(const rapidjson::Value& Object, SerializedObject*& Value,
                     std::unordered_map<GUID, SerializedObject*, GuidHasher>& ReferenceMap)
    {
        const std::string str = Object.GetString();
        GUID guid;
        StrToGuid(str, &guid);
        const auto iterator = ReferenceMap.find(guid);
        if (iterator == ReferenceMap.end())
        {
            Value = nullptr;
            return;
        }
        Value = iterator->second;
    }
} // Serialization
