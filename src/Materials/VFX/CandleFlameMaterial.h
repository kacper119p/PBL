#pragma once

#include "Materials/Material.h"
#include "Materials/Properties/MaterialProperty.h"

namespace Materials
{
    class CandleFlameMaterial final : public Material
    {
    private:
        static Shaders::Shader Shader;

    public:
        CandleFlameMaterial();

    public:

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;

#if EDITOR
        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_MATERIAL(CandleFlameMaterial)
    };
} // Models
