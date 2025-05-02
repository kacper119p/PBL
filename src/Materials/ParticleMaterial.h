#pragma once
#include "Material.h"

namespace Materials
{

    class ParticleMaterial final : public Material
    {
    private:
        static Shaders::Shader Shader;

    public:
        ParticleMaterial();

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;

#if EDITOR
        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_MATERIAL(ParticleMaterial)
    };

} // Materials
