#include "SerializationUtility.h"

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
} // Serialization
