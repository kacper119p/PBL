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
        struct BloomMip
        {
            glm::vec2 Size;
            glm::ivec2 IntSize;
            unsigned int Texture;
        };

    private:
        static constexpr size_t MipCount = 5;

        Rendering::ScreenQuad ScreenQuad;

        uint32_t FrameBuffer;
        uint32_t PrefilteredColor;
        BloomMip BloomMips[MipCount];

        Shaders::Shader PrefilterShader;
        Shaders::Shader DownSampleShader;
        Shaders::Shader UpSampleShader;
        Shaders::Shader OutputShader;

    public:
        BloomPostprocessingEffect();

    public:
        virtual ~BloomPostprocessingEffect();

    public:
        [[nodiscard]] uint32_t GetPrefilteredColor() const
        {
            return PrefilteredColor;
        }

        void Render(unsigned int SceneColorTexture);
    };

} // Engine
