#include "FlameMaterial.h"

#include "Engine/Textures/TextureManager.h"
#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

#if EDITOR
#include "imgui.h"
#endif
namespace Materials
{
    Shaders::Shader FlameMaterial::Shader;

    FlameMaterial::FlameMaterial() :
        Material(Shaders::Shader(), Shader, Shaders::Shader(), Shaders::Shader(), true),
        SpriteSheetProperty(TextureMaterialProperty("SpriteSheet", Shader,
                                                    Engine::TextureManager::GetTexture(
                                                            "res/textures/VFX/Fire/Flame.dds")))
    {
    }

    void FlameMaterial::Initialize()
    {
        Shader = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/VFX/Fire/Flame/Flame.vert",
                nullptr,
                "./res/shaders/VFX/Fire/Flame/Flame.frag"));
    }


    void FlameMaterial::UseDepthPass() const
    {
    }

    void FlameMaterial::Use() const
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        GetMainPass().Use();
        SpriteSheetProperty.Bind();
    }

    void FlameMaterial::UseDirectionalShadows() const
    {
    }

    void FlameMaterial::UsePointSpotShadows() const
    {
    }
#if EDITOR
    void FlameMaterial::DrawImGui()
    {
    }
#endif
    rapidjson::Value FlameMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        END_MATERIAL_SERIALIZATION
    }

    void FlameMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        END_MATERIAL_DESERIALIZATION
    }
} // Models
