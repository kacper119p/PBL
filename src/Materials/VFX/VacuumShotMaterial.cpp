#include "VacuumShotMaterial.h"

#include "Engine/Textures/TextureManager.h"
#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

#if EDITOR
#include "imgui.h"
#endif
namespace Materials
{
    Shaders::Shader VacuumShotMaterial::Shader;

    VacuumShotMaterial::VacuumShotMaterial() :
        Material(Shaders::Shader(), Shader, Shaders::Shader(), Shaders::Shader(), true),
        SpriteProperty(TextureMaterialProperty("Sprite", Shader,
                                               Engine::TextureManager::GetTexture(
                                                       "res/textures/VFX/Fire/Ember.dds")))
    {
    }

    void VacuumShotMaterial::Initialize()
    {
        Shader = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/VFX/VacuumShot/VacuumShot.vert",
                nullptr,
                "./res/shaders/VFX/VacuumShot/VacuumShot.frag"));
    }


    void VacuumShotMaterial::UseDepthPass() const
    {
    }

    void VacuumShotMaterial::Use() const
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        GetMainPass().Use();
        SpriteProperty.Bind();
    }

    void VacuumShotMaterial::UseDirectionalShadows() const
    {
    }

    void VacuumShotMaterial::UsePointSpotShadows() const
    {
    }
#if EDITOR
    void VacuumShotMaterial::DrawImGui()
    {
    }
#endif
    rapidjson::Value VacuumShotMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        END_MATERIAL_SERIALIZATION
    }

    void VacuumShotMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        END_MATERIAL_DESERIALIZATION
    }
} // Models
