#pragma once

#include "Materials/Material.h"
#include "Materials/Properties/MaterialProperty.h"

namespace Materials
{
    class FlameMaterial final : public Material
    {
    private:
        static Shaders::Shader Shader;

        TextureMaterialProperty SpriteSheetProperty;

    public:
        FlameMaterial();

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;

#if EDITOR
        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_MATERIAL(FlameMaterial)
    };
} // Models
