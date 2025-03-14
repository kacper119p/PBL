#include "WaterMaterial.h"
#include "GLFW/glfw3.h"

namespace Materials
{
    WaterMaterial::WaterMaterial(const Shaders::Shader& DepthPass, const Shaders::Shader& Shader,
                                 const Shaders::Shader& DirectionalShadowPass,
                                 const Shaders::Shader& PointSpotShadowPass, const unsigned int NormalMap0,
                                 const unsigned int NormalMap1, const glm::vec3& Color, const glm::vec2& Tiling0,
                                 const glm::vec2& Tiling1, const glm::vec2& Velocity0, const glm::vec2& Velocity1,
                                 const float Roughness, const float Metallic) :
        Material(DepthPass, Shader,
                 DirectionalShadowPass, PointSpotShadowPass), NormalMap0(NormalMap0),
        NormalMap1(NormalMap1), Color(Vector3MaterialProperty("Color", Shader, Color)),
        Tiling0(Vector2MaterialProperty("Tiling0", Shader, Tiling0)),
        Tiling1(Vector2MaterialProperty("Tiling1", Shader, Tiling1)),
        Velocity0(Vector2MaterialProperty("Velocity0", Shader, Velocity0)), Velocity1("Velocity1", Shader, Velocity1),
        Roughness(FloatMaterialProperty("Roughness", Shader, Roughness)),
        Metallic(FloatMaterialProperty("Metallic", Shader, Metallic)),
        TimeLocation(glGetUniformLocation(Shader.GetId(), "Time"))
    {
    }

    void WaterMaterial::UseDepthPass() const
    {
        GetDepthPass().Use();
    }

    void WaterMaterial::Use() const
    {
        GetMainPass().Use();
        Color.Bind();
        Roughness.Bind();
        Metallic.Bind();
        Tiling0.Bind();
        Tiling1.Bind();
        Velocity0.Bind();
        Velocity1.Bind();

        Shaders::Shader::SetUniform(TimeLocation, static_cast<float>(glfwGetTime()));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, NormalMap0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, NormalMap1);
    }

    void WaterMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void WaterMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use();
    }
} // Models
