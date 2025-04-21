#include <iostream>
#include "SceneFrameBuffer.h"

namespace Engine
{
    SceneFrameBuffer::SceneFrameBuffer(glm::ivec2 Resolution) :
        Resolution(Resolution)
    {
        glGenFramebuffers(1, &MultiSampledId);
        glGenFramebuffers(1, &ResolvedNormalsFramebuffer);
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

    void SceneFrameBuffer::ResolveNormals() const
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, MultiSampledId);
        glReadBuffer(GL_COLOR_ATTACHMENT1);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ResolvedNormalsFramebuffer);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(0, 0, Resolution.x, Resolution.y, 0, 0, Resolution.x, Resolution.y,
                          GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
    }

    void SceneFrameBuffer::UpdateBuffers()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, MultiSampledId);

        glDeleteTextures(1, &ColorBuffer);
        glGenTextures(1, &ColorBuffer);
        glDeleteTextures(1, &NormalsBuffer);
        glGenTextures(1, &NormalsBuffer);
        glDeleteTextures(1, &ResolvedColorBuffer);
        glGenTextures(1, &ResolvedColorBuffer);
        glDeleteRenderbuffers(1, &DepthStencilBuffer);
        glGenRenderbuffers(1, &DepthStencilBuffer);
        glDeleteTextures(1, &ResolvedNormalsTexture);
        glGenTextures(1, &ResolvedNormalsTexture);

        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ColorBuffer);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, GL_RGBA16F, Resolution.x, Resolution.y, GL_TRUE);;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, ColorBuffer, 0);

        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, NormalsBuffer);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, GL_RGBA32F, Resolution.x, Resolution.y, GL_TRUE);;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, NormalsBuffer, 0);

        constexpr uint32_t attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, attachments);

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

        glBindFramebuffer(GL_FRAMEBUFFER, ResolvedNormalsFramebuffer);

        glBindTexture(GL_TEXTURE_2D, ResolvedNormalsTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Resolution.x, Resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ResolvedNormalsTexture, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} // Rendering
