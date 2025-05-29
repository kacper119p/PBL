#include "DebugBloodMaterial.h"

#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

namespace Materials
{
    Shaders::Shader DebugBloodMaterial::Shader;

    void DebugBloodMaterial::UseDepthPass() const
    {
    }

    void DebugBloodMaterial::Use() const
    {
        Shader.Use();
    }

    void DebugBloodMaterial::UseDirectionalShadows() const
    {
    }

    void DebugBloodMaterial::UsePointSpotShadows() const
    {
    }

#if EDITOR
    void DebugBloodMaterial::DrawImGui()
    {
    }
#endif

    rapidjson::Value DebugBloodMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        END_MATERIAL_SERIALIZATION
    }

    void DebugBloodMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        END_MATERIAL_DESERIALIZATION
    }

    void DebugBloodMaterial::Initialize()
    {
        Shader = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles(
                        "./res/shaders/Blood/BloodDebug.vert",
                        nullptr,
                        "./res/shaders/Blood/BloodDebug.frag"));
    }
}
