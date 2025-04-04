#pragma once

#include "SceneFrameBuffer.h"
#include "glad/glad.h"
#include "glm/glm.hpp"

namespace Engine
{
    /**
     * @brief Multi-sampled FrameBuffer used for scene rendering.
     */
    class MultiSampledSceneFrameBuffer final
    {
    private:
        static constexpr uint8_t Samples = 8;

        uint32_t Id = 0;
        glm::ivec2 Resolution;

        uint32_t ColorBuffer = 0;
        uint32_t DepthStencilBuffer = 0;

    public:
        explicit MultiSampledSceneFrameBuffer(glm::ivec2 Resolution);

        ~MultiSampledSceneFrameBuffer();

    public:
        /**
         * @brief Returns id of the underlying FrameBuffer.
         */
        [[nodiscard]] unsigned int GetId() const
        {
            return Id;
        }

        /**
         * @brief Returns resolution of this buffer.
         */
        [[nodiscard]] const glm::ivec2& GetResolution() const
        {
            return Resolution;
        }

        /**
         * @brief Updates resolution of this FrameBuffer.
         * @param Value A new resolution.
         */
        void SetResolution(const glm::ivec2& Value)
        {
            Resolution = Value;
            UpdateBuffers();
        }

        /**
         * @brief Binds underlying FrameBuffer for rendering.
         */
        void Bind() const
        {
            glBindFramebuffer(GL_FRAMEBUFFER, Id);
        }

        /**
         * @brief Returns color texture attached to this buffer.
         */
        [[nodiscard]] uint32_t GetColorBuffer() const
        {
            return ColorBuffer;
        }

        /**
         * @brief Returns depth and stencil buffer attached to this buffer.
         */
        [[nodiscard]] uint32_t GetDepthStencilBuffer() const
        {
            return DepthStencilBuffer;
        }

        /**
         * @brief Resolves multisampling and writes result to the Target.
         * @param Target FrameBuffer to receive resolved image.
         */
        void ResolveMultisampling(const SceneFrameBuffer& Target) const;

    private:
        void UpdateBuffers();
    };
} // Rendering
