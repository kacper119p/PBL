#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Engine/Engine.h"
#include "Engine/Textures/Texture.h"
#include "rapidjson/document.h"

namespace Serialization
{
    rapidjson::Value Serialize(const glm::vec3& Value, const rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const glm::vec2& Value, const rapidjson::Document::AllocatorType& Allocator);

    rapidjson::Value Serialize(const Engine::Texture& Value, const rapidjson::Document::AllocatorType& Allocator);

    void Deserialize(const rapidjson::Value& Object, glm::vec3& Value);

    void Deserialize(const rapidjson::Value& Object, glm::vec2& Value);

    inline void Deserialize(const rapidjson::Value& Object, Engine::Texture& Value);
} // Serialization
