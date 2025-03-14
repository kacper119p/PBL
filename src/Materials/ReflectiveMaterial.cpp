#include "ReflectiveMaterial.h"

namespace Materials
{
    ReflectiveMaterial::ReflectiveMaterial(const Shaders::Shader& DepthPass, const Shaders::Shader& Shader,
                                           const Shaders::Shader& DirectionalShadowPass,
                                           const Shaders::Shader& PointSpotShadowPass,
                                           const unsigned int EnvironmentMap) :
        Material(DepthPass, Shader, DirectionalShadowPass, PointSpotShadowPass), EnvironmentMap(EnvironmentMap)
    {
    }


    void ReflectiveMaterial::UseDepthPass() const
    {
        GetDepthPass().Use();
    }

    void ReflectiveMaterial::Use() const
    {
        GetMainPass().Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, EnvironmentMap);
    }

    void ReflectiveMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void ReflectiveMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use();
    }
} // Models
