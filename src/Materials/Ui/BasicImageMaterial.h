#pragma once
#include "UiMaterial.h"

namespace Materials
{

    class BasicImageMaterial : public UiMaterial
    {
    private:
        static Shaders::Shader Shader;

    public:
        explicit BasicImageMaterial(const Shaders::Shader& Shader) :
            UiMaterial(Shader)
        {
        }

    public:
        void Use() const override;

        SERIALIZATION_EXPORT_MATERIAL(BasicImageMaterial);
    };

}
