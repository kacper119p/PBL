#pragma once

#include "Shaders/Shader.h"
#include "Material.h"
#include "Engine/Textures/Texture.h"
#include "Properties/MaterialProperty.h"
#include "Properties/TextureMaterialProperty.h"

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
        TextureMaterialProperty NormalMap0;
        TextureMaterialProperty NormalMap1;
        Vector3MaterialProperty Color;
        Vector2MaterialProperty Tiling0;
        Vector2MaterialProperty Tiling1;
        Vector2MaterialProperty Velocity0;
        Vector2MaterialProperty Velocity1;
        FloatMaterialProperty Roughness;
        FloatMaterialProperty Metallic;

        GLint TimeLocation;

    public:
        WaterMaterial(Engine::Texture NormalMap0, Engine::Texture NormalMap1, const glm::vec3& Color,
                      const glm::vec2& Tiling0, const glm::vec2& Tiling1, const glm::vec2& Velocity0,
                      const glm::vec2& Velocity1, float Roughness, float Metallic);

        WaterMaterial();

    public:
        [[nodiscard]] Engine::Texture GetNormalMap0() const
        {
            return NormalMap0.GetValue();
        }

        void SetNormalMap0(const Engine::Texture& NormalMap0)
        {
            this->NormalMap0.SetValue(NormalMap0);
        }

        [[nodiscard]] Engine::Texture GetNormalMap1() const
        {
            return NormalMap1.GetValue();
        }

        void SetNormalMap1(const Engine::Texture& NormalMap1)
        {
            this->NormalMap1.SetValue(NormalMap1);
        }

        [[nodiscard]] glm::vec3 GetColor() const
        {
            return Color.GetValue();
        }

        void SetColor(const glm::vec3& Color)
        {
            this->Color.SetValue(Color);
        }

        [[nodiscard]] glm::vec2 GetTiling0() const
        {
            return Tiling0.GetValue();
        }

        void SetTiling0(const glm::vec2& Tiling0)
        {
            this->Tiling0.SetValue(Tiling0);
        }

        [[nodiscard]] glm::vec2 GetTiling1() const
        {
            return Tiling1.GetValue();
        }

        void SetTiling1(const glm::vec2& Tiling1)
        {
            this->Tiling1.SetValue(Tiling1);
        }

        [[nodiscard]] glm::vec2 GetVelocity0() const
        {
            return Velocity0.GetValue();
        }

        void SetVelocity0(const glm::vec2& Velocity0)
        {
            this->Velocity0.SetValue(Velocity0);
        }

        [[nodiscard]] glm::vec2 GetVelocity1() const
        {
            return Velocity1.GetValue();
        }

        void SetVelocity1(const glm::vec2& Velocity1)
        {
            this->Velocity1.SetValue(Velocity1);
        }

        [[nodiscard]] float GetRoughness() const
        {
            return Roughness.GetValue();
        }

        void SetRoughness(const float& Roughness)
        {
            this->Roughness.SetValue(Roughness);
        }

        [[nodiscard]] float GetMetallic() const
        {
            return Metallic.GetValue();
        }

        void SetMetallic(const float& Metallic)
        {
            this->Metallic.SetValue(Metallic);
        }

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;

#if EDITOR
        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_MATERIAL(WaterMaterial)
    };
} // Models
