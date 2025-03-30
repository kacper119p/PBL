#include "RefractiveMaterial.h"

#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

namespace Materials
{
    const std::string RefractiveMaterial::TypeName("RefractiveMaterial");
    Shaders::Shader RefractiveMaterial::DepthPass;
    Shaders::Shader RefractiveMaterial::MainPass;
    Shaders::Shader RefractiveMaterial::DirectionalShadowPass;
    Shaders::Shader RefractiveMaterial::PointSpotShadowPass;

    RefractiveMaterial::RefractiveMaterial(const Engine::Texture EnvironmentMap, const float IndexOfRefraction) :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass),
        EnvironmentMap(EnvironmentMap),
        IndexOfRefraction(FloatMaterialProperty("IOR", MainPass, IndexOfRefraction))
    {
    }

    RefractiveMaterial::RefractiveMaterial() :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass), EnvironmentMap(Engine::Texture()),
        IndexOfRefraction(FloatMaterialProperty("IOR", MainPass))
    {
    }

    void RefractiveMaterial::Initialize()
    {
        DepthPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/DefaultDepth/DefaultDepth.vert", nullptr,
                "./res/shaders/DefaultDepth/DefaultDepth.frag"));
        MainPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Refractive/Refractive.vert", nullptr, "./res/shaders/Refractive/Refractive.frag"));
        DirectionalShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.vert", nullptr,
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.frag"));
        PointSpotShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.vert",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.geom",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.frag"));
    }


    void RefractiveMaterial::UseDepthPass() const
    {
        GetDepthPass().Use();
    }

    void RefractiveMaterial::Use() const
    {
        GetMainPass().Use();

        IndexOfRefraction.Bind();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, EnvironmentMap.GetId());
    }

    void RefractiveMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void RefractiveMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use();
    }

    rapidjson::Value RefractiveMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        SERIALIZE_PROPERTY(EnvironmentMap);
        SERIALIZE_PROPERTY(IndexOfRefraction);
        END_MATERIAL_SERIALIZATION
    }

    void RefractiveMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        DESERIALIZE_PROPERTY(EnvironmentMap);
        DESERIALIZE_PROPERTY(IndexOfRefraction);
        END_MATERIAL_DESERIALIZATION
    }
} // Models
