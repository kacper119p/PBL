#pragma once

#include "Material.h"

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
        unsigned int EnvironmentMap;

    public:
        explicit ReflectiveMaterial(unsigned int EnvironmentMap);

    public:
        [[nodiscard]] inline unsigned int GetEnvironmentMap() const
        {
            return EnvironmentMap;
        }

        inline void SetEnvironmentMap(unsigned int EnvironmentMap)
        {
            ReflectiveMaterial::EnvironmentMap = EnvironmentMap;
        }

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;
    };

} // Models
