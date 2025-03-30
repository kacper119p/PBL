#include "SkyboxMaterial.h"

#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

namespace Materials
{
    const std::string SkyboxMaterial::TypeName("SkyboxMaterial");
    Shaders::Shader SkyboxMaterial::DepthPass;
    Shaders::Shader SkyboxMaterial::MainPass;
    Shaders::Shader SkyboxMaterial::DirectionalShadowPass;
    Shaders::Shader SkyboxMaterial::PointSpotShadowPass;

    SkyboxMaterial::SkyboxMaterial(const Engine::Texture Texture) :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass), Texture(Texture)
    {
    }

    SkyboxMaterial::SkyboxMaterial():
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass), Texture(Engine::Texture())
    {
    }

    void SkyboxMaterial::Initialize()
    {
        DepthPass = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/Skybox/Skybox.vert", nullptr,
                                           "./res/shaders/Skybox/Skybox.frag"));
        MainPass = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/Skybox/Skybox.vert", nullptr,
                                           "./res/shaders/Skybox/Skybox.frag"));
        DirectionalShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Skybox/SkyboxDirectionalLightShadow.vert", nullptr,
                "./res/shaders/Skybox/SkyboxDirectionalLightShadow.frag"));
        PointSpotShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Skybox/SkyboxDirectionalLightShadow.vert", nullptr,
                "./res/shaders/Skybox/SkyboxDirectionalLightShadow.frag"));
    }

    void SkyboxMaterial::UseDepthPass() const
    {
        GetDepthPass().Use();
    }

    void SkyboxMaterial::Use() const
    {
        GetMainPass().Use();
        GetMainPass().SetTexture("Texture", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, Texture.GetId());
    }

    void SkyboxMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void SkyboxMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use();
    }

    rapidjson::Value SkyboxMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        SERIALIZE_PROPERTY(Texture);
        END_MATERIAL_SERIALIZATION
    }

    void SkyboxMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        DESERIALIZE_PROPERTY(Texture);
        END_MATERIAL_DESERIALIZATION
    }
} // Models
