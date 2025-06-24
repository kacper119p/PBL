#pragma once

#include "Materials/Material.h"
#include "Materials/Properties/MaterialProperty.h"
#include "Materials/Properties/TextureMaterialProperty.h"

namespace Materials
{
    class VacuumShotMaterial final : public Material
    {
    private:
        static Shaders::Shader Shader;

        TextureMaterialProperty SpriteProperty;
    public:
        VacuumShotMaterial();

    public:

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;

#if EDITOR
        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_MATERIAL(VacuumShotMaterial)
    };
} // Models
