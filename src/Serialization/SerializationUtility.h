#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "rapidjson/document.h"

namespace Serialization
{
    const rapidjson::Value Serialize(const glm::vec3& Value, const rapidjson::Document::AllocatorType& Allocator);

    const rapidjson::Value Serialize(const glm::vec2& Value, const rapidjson::Document::AllocatorType& Allocator);

    void Deserialize(const rapidjson::Value& Object, glm::vec3& Value);

    void Deserialize(const rapidjson::Value& Object, glm::vec2& Value);
} // Serialization
