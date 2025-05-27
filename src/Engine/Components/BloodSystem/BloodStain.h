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
        void Draw() const;

        void Start() override;

#if EDITOR
        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_CLASS(BloodStain)
    };

}
