#include "SerializationFilesUtility.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include <rapidjson/prettywriter.h>
#include <cstdio>

namespace
{
    constexpr size_t BufferSize = 65536;
}

namespace Serialization
{
    void ReadJsonFile(const char* FilePath, rapidjson::Document& Target)
    {
        FILE* file;
        const int errorNumber = fopen_s(&file, FilePath, "rb");
        //assert(errorNumber == 0); //failed to read file
        char buffer[BufferSize];
        rapidjson::FileReadStream inputStream(file, buffer, sizeof(buffer));

        Target.ParseStream(inputStream);
        fclose(file);

#if EDITOR || DEBUG
        assert(!Target.HasParseError());
        assert(Target.IsObject());
#endif
    }

    void WriteJsonFile(const char* FilePath, const rapidjson::Value& Object)
    {
        FILE* file;
        const int errorNumber = fopen_s(&file, FilePath, "wb");
        assert(errorNumber == 0); //failed to write to file
        char buffer[BufferSize];
        rapidjson::FileWriteStream outputStream(file, buffer, sizeof(buffer));
        rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(outputStream);

        Object.Accept(writer);
        fclose(file);
    }
} // Serialization
