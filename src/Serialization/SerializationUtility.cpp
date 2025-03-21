#include "SerializationUtility.h"

#include <atlbase.h>
#include <codecvt>
#include <locale>

#include "Engine/Textures/TextureManager.h"

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

    rapidjson::Value Serialize(const SerializedObject* const Value, const rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kStringType);
        GUID guid = Value->GetID();
        char guidStr[37]; // GUID is 36 chars + null terminator
        snprintf(guidStr, sizeof(guidStr),
                 "%08X-%04X-%04X-%04X-%02X%02X%02X%02X%02X%02X",
                 guid.Data1, guid.Data2, guid.Data3,
                 (guid.Data4[0] << 8) | guid.Data4[1],
                 guid.Data4[2], guid.Data4[3], guid.Data4[4],
                 guid.Data4[5], guid.Data4[6], guid.Data4[7]);
        object.SetString(guidStr, sizeof(guidStr));
        return object;
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

    void Deserialize(const rapidjson::Value& Object, SerializedObject*& Value,
                     std::unordered_map<GUID, SerializedObject*>& ReferenceMap)
    {
        std::string str = Object.GetString();
        std::wstring wStr(str.begin(), str.end());
        GUID guid;
        const HRESULT result = CLSIDFromString(wStr.c_str(), &guid);
#ifdef DEBUG
        assert(result == S_OK);
#endif
        Value = ReferenceMap[guid];
    }
} // Serialization
