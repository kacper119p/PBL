#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"

namespace Engine::Rendering
{
    /**
     * @brief Not used.
     */
    class GBuffer final
    {
    private:
        static constexpr uint8_t Samples = 8;

        uint32_t Id = 0;
        glm::ivec2 Resolution;

        uint32_t ColorBuffer = 0;
        uint32_t DepthStencilBuffer = 0;

    public:
        explicit GBuffer(glm::ivec2 Resolution);

        ~GBuffer();

    public:
        [[nodiscard]] unsigned int GetId() const
        {
            return Id;
        }

        [[nodiscard]] const glm::ivec2& GetResolution() const
        {
            return Resolution;
        }

        void SetResolution(const glm::ivec2& Value)
        {
            Resolution = Value;
            UpdateBuffers();
        }

        void Bind() const
        {
            glBindFramebuffer(GL_FRAMEBUFFER, Id);
        }

        [[nodiscard]] uint32_t GetColorBuffer() const
        {
            return ColorBuffer;
        }

        [[nodiscard]] uint32_t GetDepthStencilBuffer() const
        {
            return DepthStencilBuffer;
        }

        void ResolveMultisampling(uint32_t Target) const;

    private:
        void UpdateBuffers();
    };
} // Rendering
