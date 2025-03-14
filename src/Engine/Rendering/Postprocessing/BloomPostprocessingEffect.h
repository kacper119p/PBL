#pragma once

#include "Engine/Rendering/ScreenQuad.h"
#include "Shaders/Shader.h"

namespace Engine
{
    /**
     * @brief Class used to apply bloom effect to rendered scene.
     */
    class BloomPostprocessingEffect
    {
    private:
        Rendering::ScreenQuad ScreenQuad;

        unsigned int FrameBuffers[2];
        unsigned int ColorBuffers[2];

        Shaders::Shader FilterShader;
        Shaders::Shader BlurShaders[2];
        Shaders::Shader OutputShader;

    public:
        BloomPostprocessingEffect();

    public:
        virtual ~BloomPostprocessingEffect();

    public:
        void Render(unsigned int SceneColorTexture);
    };

} // Engine
