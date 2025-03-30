#pragma once
#include "rapidjson/document.h"

namespace Serialization
{

    void ReadJsonFile(const char* FilePath, rapidjson::Document& Target);

    void WriteJsonFile(const char* FilePath, const rapidjson::Value& Object);
} // Serialization
