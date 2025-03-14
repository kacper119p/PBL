#pragma once

#include "Material.h"

namespace Materials
{
    class SkyboxMaterial : public Material
    {
    private:
        unsigned int Texture;

    public:
        SkyboxMaterial(const Shaders::Shader& DepthPass, const Shaders::Shader& Shader,
                       const Shaders::Shader& DirectionalShadowPass, const Shaders::Shader& PointSpotShadowPass,
                       unsigned int Texture);

    public:
        [[nodiscard]] unsigned int GetTexture() const
        {
            return Texture;
        }

        void SetTexture(const unsigned int Texture)
        {
            SkyboxMaterial::Texture = Texture;
        }

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;
    };

} // Models
