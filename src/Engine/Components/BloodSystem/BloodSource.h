#pragma once
#include "BloodSourceBase.h"
#include "Engine/Components/Component.h"
#include "Engine/Textures/Texture.h"
#include "Shaders/Shader.h"

namespace Engine
{

    class BloodSource final : public BloodSourceBase

    {
    public:
        BloodSource();

    public:
        void Draw() const;

        void Start() override;

#if EDITOR
        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_CLASS(BloodSource)
    };
}
