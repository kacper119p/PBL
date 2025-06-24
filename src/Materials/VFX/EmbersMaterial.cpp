#include "EmbersMaterial.h"

#include "Engine/Textures/TextureManager.h"
#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

#if EDITOR
#include "imgui.h"
#endif
namespace Materials
{
    Shaders::Shader EmbersMaterial::Shader;

    EmbersMaterial::EmbersMaterial() :
        Material(Shaders::Shader(), Shader, Shaders::Shader(), Shaders::Shader(), true),
        SpriteProperty(TextureMaterialProperty("Sprite", Shader,
                                               Engine::TextureManager::GetTexture(
                                                       "res/textures/VFX/Fire/Ember.dds")))
    {
    }

    void EmbersMaterial::Initialize()
    {
        Shader = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/VFX/Fire/Embers/Embers.vert",
                nullptr,
                "./res/shaders/VFX/Fire/Embers/Embers.frag"));
    }


    void EmbersMaterial::UseDepthPass() const
    {
    }

    void EmbersMaterial::Use() const
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        GetMainPass().Use();
        SpriteProperty.Bind();
    }

    void EmbersMaterial::UseDirectionalShadows() const
    {
    }

    void EmbersMaterial::UsePointSpotShadows() const
    {
    }
#if EDITOR
    void EmbersMaterial::DrawImGui()
    {
    }
#endif
    rapidjson::Value EmbersMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        END_MATERIAL_SERIALIZATION
    }

    void EmbersMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        END_MATERIAL_DESERIALIZATION
    }
} // Models
