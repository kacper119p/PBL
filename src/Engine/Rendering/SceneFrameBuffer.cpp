#include <iostream>
#include "SceneFrameBuffer.h"

namespace Engine
{
    SceneFrameBuffer::SceneFrameBuffer(glm::ivec2 Resolution) :
        Resolution(Resolution)
    {
        glGenFramebuffers(1, &MultiSampledId);
        glGenFramebuffers(1, &ResolvedId);
        UpdateBuffers();
    }

    SceneFrameBuffer::~SceneFrameBuffer()
    {
        glDeleteFramebuffers(1, &MultiSampledId);
        glDeleteFramebuffers(1, &ResolvedId);
        glDeleteTextures(1, &ColorBuffer);
        glDeleteTextures(1, &ResolvedColorBuffer);
        glDeleteRenderbuffers(1, &DepthStencilBuffer);
    }

    void SceneFrameBuffer::ResolveMultisampling() const
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, MultiSampledId);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ResolvedId);
        glBlitFramebuffer(0, 0, Resolution.x, Resolution.y, 0, 0, Resolution.x, Resolution.y,
                          GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }

    void SceneFrameBuffer::UpdateBuffers()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, MultiSampledId);

        glDeleteTextures(1, &ColorBuffer);
        glGenTextures(1, &ColorBuffer);
        glDeleteTextures(1, &ResolvedColorBuffer);
        glGenTextures(1, &ResolvedColorBuffer);
        glDeleteRenderbuffers(1, &DepthStencilBuffer);
        glGenRenderbuffers(1, &DepthStencilBuffer);

        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ColorBuffer);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, GL_RGBA16F, Resolution.x, Resolution.y, GL_TRUE);;
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, ColorBuffer, 0);

        constexpr uint32_t attachments[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, attachments);

        glBindRenderbuffer(GL_RENDERBUFFER, DepthStencilBuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, Samples, GL_DEPTH24_STENCIL8, Resolution.x, Resolution.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, DepthStencilBuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, ResolvedId);

        glBindTexture(GL_TEXTURE_2D, ResolvedColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Resolution.x, Resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ResolvedColorBuffer, 0);

        glBindRenderbuffer(GL_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} // Rendering
