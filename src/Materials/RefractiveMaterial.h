#pragma once

#include "Material.h"
#include "Engine/Textures/Texture.h"
#include "Properties/MaterialProperty.h"

namespace Materials
{
    class RefractiveMaterial final : public Material
    {
    private:
        static Shaders::Shader DepthPass;
        static Shaders::Shader MainPass;
        static Shaders::Shader DirectionalShadowPass;
        static Shaders::Shader PointSpotShadowPass;

    private:
        Engine::Texture EnvironmentMap;
        FloatMaterialProperty IndexOfRefraction;

    public:
        RefractiveMaterial(Engine::Texture EnvironmentMap, float IndexOfRefraction);

        RefractiveMaterial();

    public:
        static void Initialize();

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

        SERIALIZATION_EXPORT_MATERIAL(RefractiveMaterial)
    };
} // Models
