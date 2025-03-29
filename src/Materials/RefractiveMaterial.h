#pragma once

#include "Material.h"
#include "Engine/Textures/Texture.h"
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
        Engine::Texture EnvironmentMap;
        FloatMaterialProperty IndexOfRefraction;

    public:
        RefractiveMaterial(unsigned int EnvironmentMap, float IndexOfRefraction);

        RefractiveMaterial();

    public:
        [[nodiscard]] Engine::Texture GetEnvironmentMap() const
        {
            return EnvironmentMap;
        }

        void SetEnvironmentMap(const Engine::Texture EnvironmentMap)
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

        MATERIAL_SERIALIZATION_METHODS_DECLARATIONS
    };
} // Models
