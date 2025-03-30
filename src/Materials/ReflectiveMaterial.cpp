#include "ReflectiveMaterial.h"

#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

namespace Materials
{
    Shaders::Shader ReflectiveMaterial::DepthPass;
    Shaders::Shader ReflectiveMaterial::MainPass;
    Shaders::Shader ReflectiveMaterial::DirectionalShadowPass;
    Shaders::Shader ReflectiveMaterial::PointSpotShadowPass;

    ReflectiveMaterial::ReflectiveMaterial(const unsigned int EnvironmentMap) :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass),
        EnvironmentMap(Engine::Texture(EnvironmentMap))
    {
    }

    ReflectiveMaterial::ReflectiveMaterial() :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass), EnvironmentMap(Engine::Texture())
    {
    }

    void ReflectiveMaterial::Initialize()
    {
        DepthPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/DefaultDepth/DefaultDepth.vert", nullptr,
                "./res/shaders/DefaultDepth/DefaultDepth.frag"));
        MainPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Reflective/Reflective.vert", nullptr, "./res/shaders/Reflective/Reflective.frag"));
        DirectionalShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.vert", nullptr,
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.frag"));
        PointSpotShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.vert",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.geom",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.frag"));
    }


    void ReflectiveMaterial::UseDepthPass() const
    {
        GetDepthPass().Use();
    }

    void ReflectiveMaterial::Use() const
    {
        GetMainPass().Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, EnvironmentMap.GetId());
    }

    void ReflectiveMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void ReflectiveMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use();
    }

    rapidjson::Value ReflectiveMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        SERIALIZE_PROPERTY(EnvironmentMap);
        END_MATERIAL_SERIALIZATION
    }

    void ReflectiveMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        DESERIALIZE_PROPERTY(EnvironmentMap);
        END_MATERIAL_DESERIALIZATION
    }
} // Models
