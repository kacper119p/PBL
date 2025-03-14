#include "PbrMaterial.h"

namespace Materials
{
    PbrMaterial::PbrMaterial(const Shaders::Shader& DepthPass, const Shaders::Shader& Shader,
                             const Shaders::Shader& DirectionalShadowPass, const Shaders::Shader& PointSpotShadowPass,
                             const unsigned int BaseMap, const unsigned int RoughnessMetallicMap,
                             const unsigned int NormalMap, const unsigned int EmissiveMap, const glm::vec3& BaseColor,
                             const float Roughness, const float Metallic, const glm::vec3& EmissiveColor) :
        Material(DepthPass, Shader, DirectionalShadowPass, PointSpotShadowPass), BaseMap(BaseMap),
        RoughnessMetallicMap(RoughnessMetallicMap), NormalMap(NormalMap), EmissiveMap(EmissiveMap),
        Roughness(FloatMaterialProperty("Roughness", Shader, Roughness)),
        Metallic(FloatMaterialProperty("Metallic", Shader, Metallic)),
        BaseColor(Vector3MaterialProperty("BaseColor", Shader, BaseColor)),
        EmissiveColor(Vector3MaterialProperty("EmissiveColor", Shader, EmissiveColor))
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
