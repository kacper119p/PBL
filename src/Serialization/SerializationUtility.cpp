#include "SerializationUtility.h"

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

    inline void Deserialize(const rapidjson::Value& Object, glm::vec3& Value)
    {
        Value.x = Object["x"].GetFloat();
        Value.y = Object["y"].GetFloat();
        Value.z = Object["z"].GetFloat();
    }

    inline void Deserialize(const rapidjson::Value& Object, glm::vec2& Value)
    {
        Value.x = Object["x"].GetFloat();
        Value.y = Object["y"].GetFloat();
    }
} // Serialization
