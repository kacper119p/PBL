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
        static constexpr uint8_t Samples = 16;

        uint32_t MultiSampledId = 0;
        glm::ivec2 Resolution;
        uint32_t ColorBuffer = 0;
        uint32_t NormalsBuffer = 0;
        uint32_t OcclusionBuffer = 0;
        uint32_t DepthStencilBuffer = 0;

        uint32_t ResolvedNormalsFramebuffer = 0;
        uint32_t ResolvedNormalsTexture = 0;
        uint32_t ResolvedOcclusionBuffer = 0;
        uint32_t ResolvedDepthBuffer = 0;

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

        [[nodiscard]] uint32_t GetResolvedOcclusionBuffer() const
        {
            return ResolvedOcclusionBuffer;
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
            constexpr GLenum drawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
            glDrawBuffers(2, drawBuffers);
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

        [[nodiscard]] uint32_t GetNormalsBuffer() const
        {
            return NormalsBuffer;
        }

        [[nodiscard]] uint32_t GetResolvedNormals() const
        {
            return ResolvedNormalsTexture;
        }


        /**
         * @brief Resolves multisampling and writes result to Resolved color buffer.
         */
        void ResolveMultisampling() const;

        void ResolveNormals() const;

        void WriteNormals() const
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, NormalsBuffer, 0);
        }

        void WriteOcclusion() const
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, OcclusionBuffer, 0);
        }

    private:
        void UpdateBuffers();
    };
} // Rendering
