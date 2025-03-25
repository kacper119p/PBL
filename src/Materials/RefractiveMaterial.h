#pragma once

#include "Material.h"
#include "Properties/MaterialProperty.h"

namespace Materials
{
    class RefractiveMaterial final : public Material
    {
    private:
        static const Shaders::Shader DepthPass;
        static const Shaders::Shader MainPass;
        static const Shaders::Shader DirectionalShadowPass;
        static const Shaders::Shader PointSpotShadowPass;

    private:
        unsigned int EnvironmentMap;
        FloatMaterialProperty IndexOfRefraction;

    public:
        RefractiveMaterial(unsigned int EnvironmentMap, float IndexOfRefraction);

    public:
        [[nodiscard]] unsigned int GetEnvironmentMap() const
        {
            return EnvironmentMap;
        }

        void SetEnvironmentMap(const unsigned int EnvironmentMap)
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
