#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"

namespace Engine::Rendering
{
    /**
     * @brief Not used.
     */
    class GBuffer
    {
    private:
        unsigned int id;
        glm::vec2 resolution;

        unsigned int baseColorBuffer;
        unsigned int positionAndSpecularBuffer;
        unsigned int normalAndMetallicBuffer;
        unsigned int depthStencilBuffer;

    public:
        explicit GBuffer(glm::vec2 resolution);

        virtual ~GBuffer();

    public:
        [[nodiscard]] inline unsigned int GetId() const
        {
            return id;
        }

        [[nodiscard]] inline const glm::vec2& GetResolution() const
        {
            return resolution;
        }

        inline void SetResolution(const glm::vec2& value)
        {
            GBuffer::resolution = value;
            UpdateBuffers();
        }

        inline void Bind() const
        {
            glBindFramebuffer(GL_FRAMEBUFFER, id);
        }

        void BindOutput();

    private:
        void UpdateBuffers();
    };
} // Rendering
