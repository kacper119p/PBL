#pragma once
#include <glm/vec3.hpp>

#include "BloodSourceBase.h"
#include "Engine/Textures/Texture.h"
#include "Shaders/Shader.h"


namespace Engine
{

    class BloodStain final : public BloodSourceBase
    {
    public:
        BloodStain();

    public:
        /**
         * @brief Draws blood stain to a blood mask.
         */
        void Draw() const; // Should not be virtual

        void Start() override;

#if EDITOR
        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_CLASS(BloodStain)
    };

}
