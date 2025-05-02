#include "ParticleMaterial.h"

#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"

namespace Materials
{
    Shaders::Shader ParticleMaterial::Shader;

    void ParticleMaterial::Initialize()
    {
        Shader = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles("./res/shaders/Particles/Particles.vert",
                                                                              "",
                                                                              "./res/shaders/Particles/Particle.frag"));
    }

    ParticleMaterial::ParticleMaterial() :
        Material(Shaders::Shader(), Shader, Shaders::Shader(), Shaders::Shader())
    {
    }

    void ParticleMaterial::UseDepthPass() const
    {
    }

    void ParticleMaterial::Use() const
    {
        Shader.Use();
    }

    void ParticleMaterial::UseDirectionalShadows() const
    {
    }

    void ParticleMaterial::UsePointSpotShadows() const
    {
    }

#if EDITOR
    void ParticleMaterial::DrawImGui()
    {
    }
#endif

    rapidjson::Value ParticleMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        END_MATERIAL_SERIALIZATION
    }

    void ParticleMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        END_MATERIAL_DESERIALIZATION
    }


} // Materials
