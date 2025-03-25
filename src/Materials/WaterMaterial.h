#pragma once

#include "Shaders/Shader.h"
#include "Material.h"
#include "Properties/MaterialProperty.h"

namespace Materials
{
    class WaterMaterial final : public Material
    {
    private:
        static const Shaders::Shader DepthPass;
        static const Shaders::Shader MainPass;
        static const Shaders::Shader DirectionalShadowPass;
        static const Shaders::Shader PointSpotShadowPass;

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
        WaterMaterial(unsigned int NormalMap0, unsigned int NormalMap1, const glm::vec3& Color,
                      const glm::vec2& Tiling0, const glm::vec2& Tiling1, const glm::vec2& Velocity0,
                      const glm::vec2& Velocity1, float Roughness, float Metallic);

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;
    };
} // Models
