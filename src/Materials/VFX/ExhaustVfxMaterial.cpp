#include "ExhaustVFXMaterial.h"

#include "Engine/Textures/TextureManager.h"
#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

#if EDITOR
#include "imgui.h"
#endif
namespace Materials
{
    Shaders::Shader ExhaustVFXMaterial::Shader;

    ExhaustVFXMaterial::ExhaustVFXMaterial() :
        Material(Shaders::Shader(), Shader, Shaders::Shader(), Shaders::Shader(), true),
        SpriteProperty(TextureMaterialProperty("Sprite", Shader,
                                               Engine::TextureManager::GetTexture(
                                                       "res/textures/VFX/Exhaust/Smoke.dds")))
    {
    }

    void ExhaustVFXMaterial::Initialize()
    {
        Shader = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/VFX/Exhaust/Exhaust.vert",
                nullptr,
                "./res/shaders/VFX/Exhaust/Exhaust.frag"));
    }


    void ExhaustVFXMaterial::UseDepthPass() const
    {
    }

    void ExhaustVFXMaterial::Use() const
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        GetMainPass().Use();
        SpriteProperty.Bind();
    }

    void ExhaustVFXMaterial::UseDirectionalShadows() const
    {
    }

    void ExhaustVFXMaterial::UsePointSpotShadows() const
    {
    }
#if EDITOR
    void ExhaustVFXMaterial::DrawImGui()
    {
    }
#endif
    rapidjson::Value ExhaustVFXMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        END_MATERIAL_SERIALIZATION
    }

    void ExhaustVFXMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        END_MATERIAL_DESERIALIZATION
    }
} // Models
