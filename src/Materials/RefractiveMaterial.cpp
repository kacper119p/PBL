#include "RefractiveMaterial.h"

namespace Materials
{
    RefractiveMaterial::RefractiveMaterial(const Shaders::Shader& DepthPass, const Shaders::Shader& Shader,
                                           const Shaders::Shader& DirectionalShadowPass,
                                           const Shaders::Shader& PointSpotShadowPass,
                                           const unsigned int EnvironmentMap,
                                           const float IndexOfRefraction) :
        Material(DepthPass, Shader,
                 DirectionalShadowPass,
                 PointSpotShadowPass),
        EnvironmentMap(EnvironmentMap),
        IndexOfRefraction(FloatMaterialProperty("IOR", Shader, IndexOfRefraction))
    {
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
        glBindTexture(GL_TEXTURE_CUBE_MAP, EnvironmentMap);
    }

    void RefractiveMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void RefractiveMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use();
    }
} // Models
