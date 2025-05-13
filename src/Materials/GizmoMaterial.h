#pragma once
#include "Material.h"

namespace Materials
{

    class GizmoMaterial final : public Material
    {
    private:
        static Shaders::Shader Shader;
        static Shaders::Shader DepthShader;

    public:
        GizmoMaterial() :
            Material(DepthShader, Shader, Shaders::Shader(), Shaders::Shader())
        {
        }

        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;

#if EDITOR
        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_MATERIAL(GizmoMaterial);
    };

} // Materials
