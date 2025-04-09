#include "AnimatedPbrMaterial.h"

#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

namespace Materials
{
    Shaders::Shader AnimatedPbrMaterial::DepthPass;
    Shaders::Shader AnimatedPbrMaterial::MainPass;
    Shaders::Shader AnimatedPbrMaterial::DirectionalShadowPass;
    Shaders::Shader AnimatedPbrMaterial::PointSpotShadowPass;

    AnimatedPbrMaterial::AnimatedPbrMaterial(const Engine::Texture BaseMap, const Engine::Texture RoughnessMetallicMap,
                                             const Engine::Texture NormalMap, const Engine::Texture EmissiveMap,
                                             const glm::vec3& BaseColor, const float Roughness, const float Metallic,
                                             const glm::vec3& EmissiveColor) :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass), BaseMap(Engine::Texture(BaseMap)),
        RoughnessMetallicMap(Engine::Texture(RoughnessMetallicMap)), NormalMap(Engine::Texture(NormalMap)),
        EmissiveMap(Engine::Texture(EmissiveMap)), Roughness(FloatMaterialProperty("Roughness", MainPass, Roughness)),
        Metallic(FloatMaterialProperty("Metallic", MainPass, Metallic)),
        BaseColor(Vector3MaterialProperty("BaseColor", MainPass, BaseColor)),
        EmissiveColor(Vector3MaterialProperty("EmissiveColor", MainPass, EmissiveColor))
    {
    }

    AnimatedPbrMaterial::AnimatedPbrMaterial():
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass), BaseMap(Engine::Texture()),
        RoughnessMetallicMap(Engine::Texture()), NormalMap(Engine::Texture()), EmissiveMap(Engine::Texture()),
        Roughness(FloatMaterialProperty("Roughness", MainPass)), Metallic(FloatMaterialProperty("Metallic", MainPass)),
        BaseColor(Vector3MaterialProperty("BaseColor", MainPass)),
        EmissiveColor(Vector3MaterialProperty("EmissiveColor", MainPass))
    {
    }

    void AnimatedPbrMaterial::Initialize()
    {
        DepthPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/DefaultDepth/PBRAnimated.vert", nullptr,
                "./res/shaders/DefaultDepth/DefaultDepth.frag"));
        MainPass = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/PBR/PBRAnimated.vert", nullptr,
                                           "./res/shaders/PBR/PBR.frag"));
        DirectionalShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Common/BasicShadowPass/PBRAnimated.vert", nullptr,
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.frag"));
        PointSpotShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Common/BasicShadowPass/PBRAnimated.vert",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.geom",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.frag"));
    }

    void AnimatedPbrMaterial::UseDepthPass() const
    {
        GetDepthPass().Use();
    }

    void AnimatedPbrMaterial::Use() const
    {
        GetMainPass().Use();
        BaseColor.Bind();
        EmissiveColor.Bind();
        Roughness.Bind();
        Metallic.Bind();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, BaseMap.GetId());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, RoughnessMetallicMap.GetId());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, NormalMap.GetId());
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, EmissiveMap.GetId());
    }

    void AnimatedPbrMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void AnimatedPbrMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use();
    }

    rapidjson::Value AnimatedPbrMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        SERIALIZE_PROPERTY(BaseMap);
        SERIALIZE_PROPERTY(RoughnessMetallicMap);
        SERIALIZE_PROPERTY(NormalMap);
        SERIALIZE_PROPERTY(EmissiveMap);
        SERIALIZE_PROPERTY(Roughness);
        SERIALIZE_PROPERTY(Metallic);
        SERIALIZE_PROPERTY(BaseColor);
        SERIALIZE_PROPERTY(EmissiveColor);
        END_MATERIAL_SERIALIZATION
    }

    void AnimatedPbrMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        DESERIALIZE_PROPERTY(BaseMap);
        DESERIALIZE_PROPERTY(RoughnessMetallicMap);
        DESERIALIZE_PROPERTY(NormalMap);
        DESERIALIZE_PROPERTY(EmissiveMap);
        DESERIALIZE_PROPERTY(Roughness);
        DESERIALIZE_PROPERTY(Metallic);
        DESERIALIZE_PROPERTY(BaseColor);
        DESERIALIZE_PROPERTY(EmissiveColor);
        END_MATERIAL_DESERIALIZATION
    }
}
