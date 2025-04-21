#pragma once
#include "UiMaterial.h"

namespace Materials
{

    class BasicImageMaterial : public UiMaterial
    {
    private:
        static Shaders::Shader Shader;

    public:
        explicit BasicImageMaterial() :
            UiMaterial(Shader)
        {
        }

    public:
        void Use() const override;
#if EDITOR
        void DrawImGui() override;
#endif
        SERIALIZATION_EXPORT_MATERIAL(BasicImageMaterial);
    };

}
