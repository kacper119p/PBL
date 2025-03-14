#pragma once

#include "Shaders/Shader.h"
#include "Material.h"
#include "Properties/MaterialProperty.h"

namespace Materials
{
    class WaterMaterial : public Material
    {
    private:
        unsigned int NormalMap0;
        unsigned int NormalMap1;
        Vector3MaterialProperty Color;
        Vector2MaterialProperty Tiling0;
        Vector2MaterialProperty Tiling1;
        Vector2MaterialProperty Velocity0;
        Vector2MaterialProperty Velocity1;
        FloatMaterialProperty Roughness;
        FloatMaterialProperty Metallic;

        GLint TimeLocation;

    public:
        WaterMaterial(const Shaders::Shader& DepthPass, const Shaders::Shader& Shader,
                      const Shaders::Shader& DirectionalShadowPass, const Shaders::Shader& PointSpotShadowPass,
                      unsigned int NormalMap0, unsigned int NormalMap1, const glm::vec3& Color,
                      const glm::vec2& Tiling0, const glm::vec2& Tiling1, const glm::vec2& Velocity0,
                      const glm::vec2& Velocity1, float Roughness, float Metallic);

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;
    };
} // Models
