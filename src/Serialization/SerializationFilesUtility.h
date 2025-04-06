#pragma once
#include "rapidjson/document.h"

namespace Serialization
{
    /**
     * @brief Reads json data from a given file.
     * @param FilePath File to read data from.
     * @param Target Document to save data to.
     */
    void ReadJsonFile(const char* FilePath, rapidjson::Document& Target);

    /**
     * @brief Saves json data to a file.
     * @param FilePath File to write data to.
     * @param Object Data to write.
     */
    void WriteJsonFile(const char* FilePath, const rapidjson::Value& Object);
} // Serialization
