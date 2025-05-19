#pragma once
#include <cstdint>

#include "Engine/Rendering/SceneFrameBuffer.h"
#include "Engine/Rendering/ScreenQuad.h"
#include "Shaders/Shader.h"

namespace Engine
{

    class GodRays
    {
    private:
        uint32_t FrameBuffer;
        uint32_t OutputTexture;
        glm::ivec2 Resolution;

        Shaders::Shader Shader;
        Shaders::Shader BlendShader;

        Rendering::ScreenQuad ScreenQuad;

    public:
        GodRays();

    public:
        ~GodRays();

    public:
        void Render(const SceneFrameBuffer& SceneFrameBuffer) const;
    };

} // Engine
