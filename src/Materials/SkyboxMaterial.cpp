#include "SkyboxMaterial.h"

#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

namespace Materials
{
    const Shaders::Shader SkyboxMaterial::DepthPass = Shaders::ShaderManager::GetShader(
            Shaders::ShaderSourceFiles("./res/shaders/Skybox/Skybox.vert", nullptr,
                                       "./res/shaders/Skybox/Skybox.frag"));
    const Shaders::Shader SkyboxMaterial::MainPass = Shaders::ShaderManager::GetShader(
            Shaders::ShaderSourceFiles("./res/shaders/Skybox/Skybox.vert", nullptr,
                                       "./res/shaders/Skybox/Skybox.frag"));
    const Shaders::Shader SkyboxMaterial::DirectionalShadowPass = Shaders::ShaderManager::GetShader(
            Shaders::ShaderSourceFiles("./res/shaders/Skybox/SkyboxDirectionalLightShadow.vert", nullptr,
                                       "./res/shaders/Skybox/SkyboxDirectionalLightShadow.frag"));
    const Shaders::Shader SkyboxMaterial::PointSpotShadowPass = Shaders::ShaderManager::GetShader(
            Shaders::ShaderSourceFiles("./res/shaders/Skybox/SkyboxDirectionalLightShadow.vert", nullptr,
                                       "./res/shaders/Skybox/SkyboxDirectionalLightShadow.frag"));

    SkyboxMaterial::SkyboxMaterial(const unsigned int Texture) :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass), Texture(Engine::Texture(Texture))
    {
    }

    SkyboxMaterial::SkyboxMaterial():
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass), Texture(Engine::Texture())
    {
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
