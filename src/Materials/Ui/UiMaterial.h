#pragma once
#include "Materials/Material.h"

namespace Materials
{
    class UiMaterial : public Material
    {
    protected:
        explicit UiMaterial(const Shaders::Shader& Shader) :
            Material(Shaders::Shader(), Shader, Shaders::Shader(), Shaders::Shader())
        {
        }

    public:
        ~UiMaterial() override = default;

    public:
        void UseDepthPass() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;
    };
}
