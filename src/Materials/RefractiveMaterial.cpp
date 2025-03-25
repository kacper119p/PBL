#include "RefractiveMaterial.h"

#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

namespace Materials
{
    const Shaders::Shader RefractiveMaterial::DepthPass = Shaders::ShaderManager::GetShader(
            Shaders::ShaderSourceFiles("./res/shaders/DefaultDepth/DefaultDepth.vert", nullptr,
                                       "./res/shaders/DefaultDepth/DefaultDepth.frag"));
    const Shaders::Shader RefractiveMaterial::MainPass = Shaders::ShaderManager::GetShader(
            Shaders::ShaderSourceFiles("./res/shaders/Reflective/Reflective.vert", nullptr,
                                       "./res/shaders/Reflective/Reflective.frag"));
    const Shaders::Shader RefractiveMaterial::DirectionalShadowPass = Shaders::ShaderManager::GetShader(
            Shaders::ShaderSourceFiles("./res/shaders/Common/BasicShadowPass/DirectionalLight.vert", nullptr,
                                       "./res/shaders/Common/BasicShadowPass/DirectionalLight.frag"));
    const Shaders::Shader RefractiveMaterial::PointSpotShadowPass = Shaders::ShaderManager::GetShader(
            Shaders::ShaderSourceFiles("./res/shaders/Common/BasicShadowPass/PointSpotLight.vert",
                                       "./res/shaders/Common/BasicShadowPass/PointSpotLight.geom",
                                       "./res/shaders/Common/BasicShadowPass/PointSpotLight.frag"));

    RefractiveMaterial::RefractiveMaterial(const unsigned int EnvironmentMap, const float IndexOfRefraction) :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass), EnvironmentMap(EnvironmentMap),
        IndexOfRefraction(FloatMaterialProperty("IOR", MainPass, IndexOfRefraction))
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
