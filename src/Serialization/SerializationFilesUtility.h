#pragma once
#include "rapidjson/document.h"

namespace Serialization
{

    rapidjson::Value ReadJsonFile(const char* FilePath);

    void WriteJsonFile(const char* FilePath, rapidjson::Value& Object);
} // Serialization
