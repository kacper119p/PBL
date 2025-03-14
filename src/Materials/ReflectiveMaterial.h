#pragma once

#include "Material.h"

namespace Materials
{

    class ReflectiveMaterial : public Material
    {
    private:
        unsigned int EnvironmentMap;

    public:
        ReflectiveMaterial(const Shaders::Shader& DepthPass, const Shaders::Shader& Shader,
                           const Shaders::Shader& DirectionalShadowPass, const Shaders::Shader& PointSpotShadowPass,
                           unsigned int EnvironmentMap);

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
