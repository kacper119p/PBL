#include "BasicImageMaterial.h"

#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

namespace Materials
{
    Shaders::Shader BasicImageMaterial::Shader;

    void BasicImageMaterial::Use() const
    {
        Shader.Use();
    }

    rapidjson::Value BasicImageMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        END_MATERIAL_SERIALIZATION
    }

    void BasicImageMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        END_MATERIAL_DESERIALIZATION
    }

    void BasicImageMaterial::Initialize()
    {
        Shader = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/BasicImage/BasicImage.vert", nullptr,
                                           "./res/shaders/BasicImage/BasicImage.frag"));
    }
}
