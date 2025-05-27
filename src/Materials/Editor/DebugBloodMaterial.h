#pragma once
#include "Materials/Material.h"

namespace Materials
{
    class DebugBloodMaterial final : public Material
    {
    private:
        static Shaders::Shader Shader;

    public:
        DebugBloodMaterial() :
            Material(Shaders::Shader(), Shader, Shaders::Shader(), Shaders::Shader(), true)
        {
        }

        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;

#if EDITOR
        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_MATERIAL(DebugBloodMaterial);
    };

}
