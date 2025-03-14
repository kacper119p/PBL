#include "SkyboxMaterial.h"

namespace Materials
{
    SkyboxMaterial::SkyboxMaterial(const Shaders::Shader& DepthPass, const Shaders::Shader& Shader,
                                   const Shaders::Shader& DirectionalShadowPass,
                                   const Shaders::Shader& PointSpotShadowPass, const unsigned int Texture) :
        Material(DepthPass, Shader, DirectionalShadowPass, PointSpotShadowPass), Texture(Texture)
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
        glBindTexture(GL_TEXTURE_CUBE_MAP, Texture);
    }

    void SkyboxMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void SkyboxMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use();
    }
} // Models
