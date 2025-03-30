#pragma once

#include "Shaders/Shader.h"
#include "Material.h"
#include "Engine/Textures/Texture.h"
#include "Properties/MaterialProperty.h"

namespace Materials
{
    class WaterMaterial final : public Material
    {
    private:
        static Shaders::Shader DepthPass;
        static Shaders::Shader MainPass;
        static Shaders::Shader DirectionalShadowPass;
        static Shaders::Shader PointSpotShadowPass;

    private:
        Engine::Texture NormalMap0;
        Engine::Texture NormalMap1;
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

        WaterMaterial();

    public:
        static void Initialize();

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;

        MATERIAL_SERIALIZATION_METHODS_DECLARATIONS
    };
} // Models
