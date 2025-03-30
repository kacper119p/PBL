#pragma once

#include "Material.h"
#include "Engine/Textures/Texture.h"

namespace Materials
{
    class SkyboxMaterial final : public Material
    {
    public:
        static const std::string TypeName;

    private:
        static Shaders::Shader DepthPass;
        static Shaders::Shader MainPass;
        static Shaders::Shader DirectionalShadowPass;
        static Shaders::Shader PointSpotShadowPass;

    private:
        Engine::Texture Texture;

    public:
        explicit SkyboxMaterial(Engine::Texture Texture);

        SkyboxMaterial();

    public:
        static void Initialize();

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

        std::string GetType() const override
        {
            return TypeName;
        }

        MATERIAL_SERIALIZATION_METHODS_DECLARATIONS
    };

} // Models
