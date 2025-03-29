#include "SerializationFilesUtility.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include <rapidjson/writer.h>
#include <cstdio>

namespace
{
    constexpr size_t BufferSize = 65536;
}

namespace Serialization
{
    rapidjson::Value ReadJsonFile(const char* FilePath)
    {
        FILE* file;
        const int errorNumber = fopen_s(&file, FilePath, "rb");
        assert(errorNumber == 0); //failed to read file
        char buffer[BufferSize];
        rapidjson::FileReadStream inputStream(file, buffer, sizeof(buffer));

        rapidjson::Document document;
        document.ParseStream(inputStream);
        fclose(file);

#if EDITOR || DEBUG
        assert(!document.HasParseError());
        assert(document.IsObject());
#endif
        return rapidjson::Value(document.GetObject());
    }

    void WriteJsonFile(const char* FilePath, rapidjson::Value& Object)
    {
        FILE* file;
        const int errorNumber = fopen_s(&file, FilePath, "wb");
        assert(errorNumber == 0); //failed to write to file
        char buffer[BufferSize];
        rapidjson::FileWriteStream outputStream(file, buffer, sizeof(buffer));
        rapidjson::Writer<rapidjson::FileWriteStream> writer(outputStream);

        Object.Accept(writer);
        fclose(file);
    }
} // Serialization
