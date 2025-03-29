#pragma once

#include "Material.h"
#include "Engine/Textures/Texture.h"

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
        Engine::Texture Texture;

    public:
        explicit SkyboxMaterial(unsigned int Texture);

        SkyboxMaterial();

    public:
        [[nodiscard]] Engine::Texture GetTexture() const
        {
            return Texture;
        }

        void SetTexture(const Engine::Texture Texture)
        {
            SkyboxMaterial::Texture = Texture;
        }

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;

        MATERIAL_SERIALIZATION_METHODS_DECLARATIONS
    };

} // Models
