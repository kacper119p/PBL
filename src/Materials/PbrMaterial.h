#pragma once

#include "Shaders/Shader.h"
#include "Material.h"
#include "Properties/MaterialProperty.h"

namespace Materials
{
    class PbrMaterial : public Material
    {
    private:
        unsigned int BaseMap;
        unsigned int RoughnessMetallicMap;
        unsigned int NormalMap;
        unsigned int EmissiveMap;
        FloatMaterialProperty Roughness;
        FloatMaterialProperty Metallic;
        Vector3MaterialProperty BaseColor;
        Vector3MaterialProperty EmissiveColor;

    public:
        PbrMaterial(const Shaders::Shader& DepthPass, const Shaders::Shader& Shader,
                    const Shaders::Shader& DirectionalShadowPass, const Shaders::Shader& PointSpotShadowPass,
                    unsigned int BaseMap, unsigned int RoughnessMetallicMap, unsigned int NormalMap,
                    unsigned int EmissiveMap, const glm::vec3& BaseColor, float Roughness, float Metallic,
                    const glm::vec3& EmissiveColor);

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;

    public:
        [[nodiscard]] unsigned int GetBaseMap() const
        {
            return BaseMap;
        }

        void SetBaseMap(const unsigned int BaseMap)
        {
            PbrMaterial::BaseMap = BaseMap;
        }

        [[nodiscard]] unsigned int GetRoughnessMetallicMap() const
        {
            return RoughnessMetallicMap;
        }

        void SetRoughnessMetallicMap(const unsigned int RoughnessMetallicMap)
        {
            PbrMaterial::RoughnessMetallicMap = RoughnessMetallicMap;
        }

        [[nodiscard]] unsigned int GetNormalMap() const
        {
            return NormalMap;
        }

        void SetNormalMap(const unsigned int NormalMap)
        {
            PbrMaterial::NormalMap = NormalMap;
        }

        [[nodiscard]] unsigned int GetEmissiveMap() const
        {
            return EmissiveMap;
        }

        void SetEmissiveMap(const unsigned int EmissiveMap)
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
    };
} // Models
