#include "PbrMaterial.h"

#include "Shaders/ShaderManager.h"

namespace Materials
{
    const Shaders::Shader PbrMaterial::DepthPass = Shaders::ShaderManager::GetShader(
            Shaders::ShaderSourceFiles("./res/shaders/DefaultDepth/DefaultDepth.vert", nullptr,
                                       "./res/shaders/DefaultDepth/DefaultDepth.frag"));
    const Shaders::Shader PbrMaterial::MainPass = Shaders::ShaderManager::GetShader(
            Shaders::ShaderSourceFiles("./res/shaders/PBR/PBR.vert", nullptr, "./res/shaders/PBR/PBR.frag"));
    const Shaders::Shader PbrMaterial::DirectionalShadowPass = Shaders::ShaderManager::GetShader(
            Shaders::ShaderSourceFiles("./res/shaders/Common/BasicShadowPass/DirectionalLight.vert", nullptr,
                                       "./res/shaders/Common/BasicShadowPass/DirectionalLight.frag"));
    const Shaders::Shader PbrMaterial::PointSpotShadowPass = Shaders::ShaderManager::GetShader(
            Shaders::ShaderSourceFiles("./res/shaders/Common/BasicShadowPass/PointSpotLight.vert",
                                       "./res/shaders/Common/BasicShadowPass/PointSpotLight.geom",
                                       "./res/shaders/Common/BasicShadowPass/PointSpotLight.frag"));

    PbrMaterial::PbrMaterial(const unsigned int BaseMap, const unsigned int RoughnessMetallicMap,
                             const unsigned int NormalMap, const unsigned int EmissiveMap, const glm::vec3& BaseColor,
                             const float Roughness, const float Metallic, const glm::vec3& EmissiveColor) :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass), BaseMap(BaseMap),
        RoughnessMetallicMap(RoughnessMetallicMap), NormalMap(NormalMap), EmissiveMap(EmissiveMap),
        Roughness(FloatMaterialProperty("Roughness", MainPass, Roughness)),
        Metallic(FloatMaterialProperty("Metallic", MainPass, Metallic)),
        BaseColor(Vector3MaterialProperty("BaseColor", MainPass, BaseColor)),
        EmissiveColor(Vector3MaterialProperty("EmissiveColor", MainPass, EmissiveColor))
    {
    }

    void PbrMaterial::UseDepthPass() const
    {
        GetDepthPass().Use();
    }

    void PbrMaterial::Use() const
    {
        GetMainPass().Use();
        BaseColor.Bind();
        EmissiveColor.Bind();
        Roughness.Bind();
        Metallic.Bind();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, BaseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, RoughnessMetallicMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, NormalMap);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, EmissiveMap);
    }

    void PbrMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void PbrMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use();
    }
}
