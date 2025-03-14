#pragma once

#include "Material.h"
#include "Properties/MaterialProperty.h"

namespace Materials
{
    class RefractiveMaterial : public Material
    {
    private:
        unsigned int EnvironmentMap;
        FloatMaterialProperty IndexOfRefraction;

    public:
        RefractiveMaterial(const Shaders::Shader& DepthPass, const Shaders::Shader& Shader,
                           const Shaders::Shader& DirectionalShadowPass,
                           const Shaders::Shader& PointSpotShadowPass, unsigned int EnvironmentMap,
                           float IndexOfRefraction);

    public:
        [[nodiscard]] unsigned int GetEnvironmentMap() const
        {
            return EnvironmentMap;
        }

        void SetEnvironmentMap(unsigned int EnvironmentMap)
        {
            RefractiveMaterial::EnvironmentMap = EnvironmentMap;
        }

        [[nodiscard]] float GetIndexOfRefraction() const
        {
            return IndexOfRefraction.GetValue();
        }

        void SetIndexOfRefraction(float IndexOfRefraction)
        {
            RefractiveMaterial::IndexOfRefraction.SetValue(IndexOfRefraction);
        }

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;
    };
} // Models
