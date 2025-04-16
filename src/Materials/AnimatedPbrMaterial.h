#pragma once
#include "Material.h"
#include "Engine/Textures/Texture.h"
#include "Properties/MaterialProperty.h"
#include "Shaders/Shader.h"

namespace Materials
{

    class AnimatedPbrMaterial final : public Material
    {
    private:
        static Shaders::Shader DepthPass;
        static Shaders::Shader MainPass;
        static Shaders::Shader DirectionalShadowPass;
        static Shaders::Shader PointSpotShadowPass;

    private:
        Engine::Texture BaseMap;
        Engine::Texture RoughnessMetallicMap;
        Engine::Texture NormalMap;
        Engine::Texture EmissiveMap;
        FloatMaterialProperty Roughness;
        FloatMaterialProperty Metallic;
        Vector3MaterialProperty BaseColor;
        Vector3MaterialProperty EmissiveColor;

    public:
        AnimatedPbrMaterial(Engine::Texture BaseMap, Engine::Texture RoughnessMetallicMap, Engine::Texture NormalMap,
                            Engine::Texture EmissiveMap, const glm::vec3& BaseColor, float Roughness, float Metallic,
                            const glm::vec3& EmissiveColor);

        AnimatedPbrMaterial();

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;

    public:
        [[nodiscard]] Engine::Texture GetBaseMap() const
        {
            return BaseMap;
        }

        void SetBaseMap(const Engine::Texture BaseMap)
        {
            AnimatedPbrMaterial::BaseMap = BaseMap;
        }

        [[nodiscard]] Engine::Texture GetRoughnessMetallicMap() const
        {
            return RoughnessMetallicMap;
        }

        void SetRoughnessMetallicMap(const Engine::Texture RoughnessMetallicMap)
        {
            AnimatedPbrMaterial::RoughnessMetallicMap = RoughnessMetallicMap;
        }

        [[nodiscard]] Engine::Texture GetNormalMap() const
        {
            return NormalMap;
        }

        void SetNormalMap(const Engine::Texture NormalMap)
        {
            AnimatedPbrMaterial::NormalMap = NormalMap;
        }

        [[nodiscard]] Engine::Texture GetEmissiveMap() const
        {
            return EmissiveMap;
        }

        void SetEmissiveMap(const Engine::Texture EmissiveMap)
        {
            AnimatedPbrMaterial::EmissiveMap = EmissiveMap;
        }

        [[nodiscard]] glm::vec3 GetBaseColor() const
        {
            return BaseColor.GetValue();
        }

        void SetBaseColor(const glm::vec3& BaseColor)
        {
            AnimatedPbrMaterial::BaseColor.SetValue(BaseColor);
        }

        [[nodiscard]] float GetRoughness() const
        {
            return Roughness.GetValue();
        }

        void SetRoughness(const float Roughness)
        {
            AnimatedPbrMaterial::Roughness.SetValue(Roughness);
        }

        [[nodiscard]] float GetMetallic() const
        {
            return Metallic.GetValue();
        }

        void SetMetallic(const float Metallic)
        {
            AnimatedPbrMaterial::Metallic.SetValue(Metallic);
        }

        [[nodiscard]] glm::vec3 GetEmissiveColor() const
        {
            return EmissiveColor.GetValue();
        }

        void SetEmissiveColor(const glm::vec3& EmissiveColor)
        {
            AnimatedPbrMaterial::EmissiveColor.SetValue(EmissiveColor);
        }

        SERIALIZATION_EXPORT_MATERIAL(AnimatedPbrMaterial)
    };

} // Materials
