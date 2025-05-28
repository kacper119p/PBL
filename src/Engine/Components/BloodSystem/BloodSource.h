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
        /**
         * @brief Draws blood source to a blood mask.
         */
        void Draw() const; // Should not be virtual.

        void Start() override;

#if EDITOR
        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_CLASS(BloodSource)
    };
}
