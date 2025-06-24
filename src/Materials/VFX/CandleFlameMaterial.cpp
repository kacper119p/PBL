#include "CandleFlameMaterial.h"

#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

#if EDITOR
#include "imgui.h"
#endif
namespace Materials
{
    Shaders::Shader CandleFlameMaterial::Shader;

    CandleFlameMaterial::CandleFlameMaterial() :
        Material(Shaders::Shader(), Shader, Shaders::Shader(), Shaders::Shader(), true)
    {
    }

    void CandleFlameMaterial::Initialize()
    {
        Shader = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/VFX/CandleFlame/CandleFlame.vert",
                nullptr,
                "./res/shaders/VFX/CandleFlame/CandleFlame.frag"));
    }


    void CandleFlameMaterial::UseDepthPass() const
    {
    }

    void CandleFlameMaterial::Use() const
    {
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        GetMainPass().Use();
    }

    void CandleFlameMaterial::UseDirectionalShadows() const
    {
    }

    void CandleFlameMaterial::UsePointSpotShadows() const
    {
    }
#if EDITOR
    void CandleFlameMaterial::DrawImGui()
    {
    }
#endif
    rapidjson::Value CandleFlameMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        END_MATERIAL_SERIALIZATION
    }

    void CandleFlameMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        END_MATERIAL_DESERIALIZATION
    }
} // Models
