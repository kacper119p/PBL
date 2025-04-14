#pragma once

#include "SceneFrameBuffer.h"
#include "glad/glad.h"
#include "glm/glm.hpp"

namespace Engine
{
    /**
     * @brief Multi-sampled FrameBuffer used for scene rendering.
     */
    class SceneFrameBuffer final
    {
    private:
        static constexpr uint8_t Samples = 32;

        uint32_t MultiSampledId = 0;
        glm::ivec2 Resolution;
        uint32_t ColorBuffer = 0;
        uint32_t NormalsBuffer = 0;
        uint32_t DepthStencilBuffer = 0;

        uint32_t ResolvedId = 0;
        uint32_t ResolvedColorBuffer = 0;

    public:
        explicit SceneFrameBuffer(glm::ivec2 Resolution);

        ~SceneFrameBuffer();

    public:
        /**
         * @brief Returns id of the underlying FrameBuffer.
         */
        [[nodiscard]] unsigned int GetMultiSampledId() const
        {
            return MultiSampledId;
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
        void BindMultiSampled() const
        {
            glBindFramebuffer(GL_FRAMEBUFFER, MultiSampledId);
        }

        void BindResolved() const
        {
            glBindFramebuffer(GL_FRAMEBUFFER, ResolvedId);
        }

        /**
         * @brief Returns color texture attached to this buffer.
         */
        [[nodiscard]] uint32_t GetMultiSampledColorBuffer() const
        {
            return ColorBuffer;
        }

        [[nodiscard]] uint32_t GetResolvedColorBuffer() const
        {
            return ResolvedColorBuffer;
        }

        /**
         * @brief Returns depth and stencil buffer attached to this buffer.
         */
        [[nodiscard]] uint32_t GetDepthStencilBuffer() const
        {
            return DepthStencilBuffer;
        }

        /**
         * @brief Resolves multisampling and writes result to Resolved color buffer.
         */
        void ResolveMultisampling() const;

    private:
        void UpdateBuffers();
    };
} // Rendering
