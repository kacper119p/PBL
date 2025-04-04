#pragma once

#include <cstdint>

#include "glad/glad.h"
#include "glm/glm.hpp"


namespace Engine
{
    /**
     * @brief Framebuffer used for scene rendering.
     */
    class SceneFrameBuffer
    {
    private:
        static constexpr uint8_t Samples = 8;

        uint32_t Id = 0;
        glm::ivec2 Resolution;

        uint32_t ColorBuffer = 0;
        uint32_t DepthStencilBuffer = 0;

    public:
        explicit SceneFrameBuffer(glm::ivec2 Resolution);

        ~SceneFrameBuffer();

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

    private:
        void UpdateBuffers();
    };

} // Engine
