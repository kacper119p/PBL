#pragma once

#include "Shaders/Shader.h"
#include "Material.h"
#include "Engine/Textures/Texture.h"
#include "Properties/MaterialProperty.h"

namespace Materials
{
    class PbrMaterial final : public Material
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
        PbrMaterial(Engine::Texture BaseMap, Engine::Texture RoughnessMetallicMap, Engine::Texture NormalMap,
                    Engine::Texture EmissiveMap, const glm::vec3& BaseColor, float Roughness, float Metallic,
                    const glm::vec3& EmissiveColor);

        PbrMaterial();

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;
#if EDITOR
        void DrawImGui() override;
#endif

    public:
        [[nodiscard]] Engine::Texture GetBaseMap() const
        {
            return BaseMap;
        }

        void SetBaseMap(const Engine::Texture BaseMap)
        {
            PbrMaterial::BaseMap = BaseMap;
        }

        [[nodiscard]] Engine::Texture GetRoughnessMetallicMap() const
        {
            return RoughnessMetallicMap;
        }

        void SetRoughnessMetallicMap(const Engine::Texture RoughnessMetallicMap)
        {
            PbrMaterial::RoughnessMetallicMap = RoughnessMetallicMap;
        }

        [[nodiscard]] Engine::Texture GetNormalMap() const
        {
            return NormalMap;
        }

        void SetNormalMap(const Engine::Texture NormalMap)
        {
            PbrMaterial::NormalMap = NormalMap;
        }

        [[nodiscard]] Engine::Texture GetEmissiveMap() const
        {
            return EmissiveMap;
        }

        void SetEmissiveMap(const Engine::Texture EmissiveMap)
        {
            PbrMaterial::EmissiveMap = EmissiveMap;
        }

        [[nodiscard]] glm::vec3 GetBaseColor() const
        {
            return BaseColor.GetValue();
        }

        void SetBaseColor(const glm::vec3& BaseColor)
        {
            PbrMaterial::BaseColor.SetValue(BaseColor);
        }

        [[nodiscard]] float GetRoughness() const
        {
            return Roughness.GetValue();
        }

        void SetRoughness(const float Roughness)
        {
            PbrMaterial::Roughness.SetValue(Roughness);
        }

        [[nodiscard]] float GetMetallic() const
        {
            return Metallic.GetValue();
        }

        void SetMetallic(const float Metallic)
        {
            PbrMaterial::Metallic.SetValue(Metallic);
        }

        [[nodiscard]] glm::vec3 GetEmissiveColor() const
        {
            return EmissiveColor.GetValue();
        }

        void SetEmissiveColor(const glm::vec3& EmissiveColor)
        {
            PbrMaterial::EmissiveColor.SetValue(EmissiveColor);
        }

        SERIALIZATION_EXPORT_MATERIAL(PbrMaterial)
    };
} // Models
