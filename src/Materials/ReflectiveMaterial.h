#pragma once

#include "Material.h"
#include "Engine/Textures/Texture.h"

namespace Materials
{

    class ReflectiveMaterial final : public Material
    {
    private:
        static const Shaders::Shader DepthPass;
        static const Shaders::Shader MainPass;
        static const Shaders::Shader DirectionalShadowPass;
        static const Shaders::Shader PointSpotShadowPass;

    private:
        Engine::Texture EnvironmentMap;

    public:
        explicit ReflectiveMaterial(unsigned int EnvironmentMap);

        ReflectiveMaterial();

    public:
        [[nodiscard]] Engine::Texture GetEnvironmentMap() const
        {
            return EnvironmentMap;
        }

        inline void SetEnvironmentMap(Engine::Texture EnvironmentMap)
        {
            ReflectiveMaterial::EnvironmentMap = EnvironmentMap;
        }

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;

        MATERIAL_SERIALIZATION_METHODS_DECLARATIONS
    };

} // Models
