#include "VacuumVFXMaterial.h"

#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

#if EDITOR
#include "imgui.h"
#endif
namespace Materials
{
    Shaders::Shader VacuumVfxMaterial::Shader;

    VacuumVfxMaterial::VacuumVfxMaterial() :
        Material(Shaders::Shader(), Shader, Shaders::Shader(), Shaders::Shader(), true)
    {
    }

    void VacuumVfxMaterial::Initialize()
    {
        Shader = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/VFX/Vacuum/Vacuum.vert",
                nullptr,
                "./res/shaders/VFX/Vacuum/Vacuum.frag"));
    }


    void VacuumVfxMaterial::UseDepthPass() const
    {
    }

    void VacuumVfxMaterial::Use() const
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        GetMainPass().Use();
    }

    void VacuumVfxMaterial::UseDirectionalShadows() const
    {
    }

    void VacuumVfxMaterial::UsePointSpotShadows() const
    {
    }
#if EDITOR
    void VacuumVfxMaterial::DrawImGui()
    {
    }
#endif
    rapidjson::Value VacuumVfxMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        END_MATERIAL_SERIALIZATION
    }

    void VacuumVfxMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        END_MATERIAL_DESERIALIZATION
    }
} // Models
