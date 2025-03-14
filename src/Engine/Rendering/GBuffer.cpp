#include <iostream>
#include "GBuffer.h"

namespace Engine::Rendering
{
    GBuffer::GBuffer(glm::vec2 resolution) : resolution(resolution), baseColorBuffer(0),
                                             positionAndSpecularBuffer(0),
                                             normalAndMetallicBuffer(0), depthStencilBuffer(0)
    {
        glGenFramebuffers(1, &id);
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        UpdateBuffers();
    }

    GBuffer::~GBuffer()
    {
        glDeleteFramebuffers(1, &id);
        glDeleteRenderbuffers(1, &depthStencilBuffer);
        glDeleteTextures(3, &baseColorBuffer);
    }

    void GBuffer::UpdateBuffers()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);

        glDeleteTextures(3, &baseColorBuffer);
        glGenTextures(3, &baseColorBuffer);
        glDeleteRenderbuffers(1, &depthStencilBuffer);
        glGenRenderbuffers(1, &depthStencilBuffer);

        glBindTexture(GL_TEXTURE_2D, baseColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, baseColorBuffer, 0);

        glBindTexture(GL_TEXTURE_2D, positionAndSpecularBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, positionAndSpecularBuffer, 0);

        glBindTexture(GL_TEXTURE_2D, normalAndMetallicBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, normalAndMetallicBuffer, 0);

        unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                                       GL_COLOR_ATTACHMENT2};
        glDrawBuffers(3, attachments);

        glBindRenderbuffer(GL_RENDERBUFFER, depthStencilBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolution.x, resolution.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilBuffer);

        glBindRenderbuffer(GL_FRAMEBUFFER, 0);
    }

    void GBuffer::BindOutput()
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, baseColorBuffer);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, positionAndSpecularBuffer);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normalAndMetallicBuffer);
    }
} // Rendering