#pragma once

#include "Material.h"

namespace Materials
{
    class SkyboxMaterial final : public Material
    {
    private:
        static const Shaders::Shader DepthPass;
        static const Shaders::Shader MainPass;
        static const Shaders::Shader DirectionalShadowPass;
        static const Shaders::Shader PointSpotShadowPass;

    private:
        unsigned int Texture;

    public:
        explicit SkyboxMaterial(unsigned int Texture);

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
