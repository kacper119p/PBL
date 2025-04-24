#pragma once

#include "Material.h"
#include "Engine/Textures/Texture.h"
#include "Properties/TextureMaterialProperty.h"

namespace Materials
{
    class SkyboxMaterial final : public Material
    {
    private:
        static Shaders::Shader DepthPass;
        static Shaders::Shader MainPass;
        static Shaders::Shader DirectionalShadowPass;
        static Shaders::Shader PointSpotShadowPass;

    private:
        TextureMaterialProperty Texture;

    public:
        explicit SkyboxMaterial(Engine::Texture Texture);

        SkyboxMaterial();

    public:
        [[nodiscard]] Engine::Texture GetTexture() const
        {
            return Texture.GetValue();
        }

        void SetTexture(const Engine::Texture Texture)
        {
            SkyboxMaterial::Texture.SetValue(Texture);
        }

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;
#if EDITOR
        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_MATERIAL(SkyboxMaterial);
    };

} // Models
