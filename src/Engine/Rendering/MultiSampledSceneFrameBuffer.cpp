#include <iostream>
#include "MultiSampledSceneFrameBuffer.h"

namespace Engine
{
    MultiSampledSceneFrameBuffer::MultiSampledSceneFrameBuffer(glm::ivec2 Resolution) :
        Resolution(Resolution)
    {
        glGenFramebuffers(1, &Id);
        UpdateBuffers();
    }

    MultiSampledSceneFrameBuffer::~MultiSampledSceneFrameBuffer()
    {
        glDeleteFramebuffers(1, &Id);
        glDeleteTextures(1, &ColorBuffer);
        glDeleteRenderbuffers(1, &DepthStencilBuffer);
    }

    void MultiSampledSceneFrameBuffer::ResolveMultisampling(const SceneFrameBuffer& Target) const
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, Id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Target.GetId());
        glBlitFramebuffer(0, 0, Resolution.x, Resolution.y, 0, 0, Resolution.x, Resolution.y,
                          GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glBlitFramebuffer(0, 0, Resolution.x, Resolution.y, 0, 0, Resolution.x, Resolution.y,
                          GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    void MultiSampledSceneFrameBuffer::UpdateBuffers()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, Id);

        glDeleteTextures(1, &ColorBuffer);
        glGenTextures(1, &ColorBuffer);
        glDeleteRenderbuffers(1, &DepthStencilBuffer);
        glGenRenderbuffers(1, &DepthStencilBuffer);

        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ColorBuffer);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, GL_RGBA16F, Resolution.x, Resolution.y, GL_TRUE);;
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, ColorBuffer, 0);

        constexpr uint32_t attachments[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, attachments);

        glBindRenderbuffer(GL_RENDERBUFFER, DepthStencilBuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, Samples, GL_DEPTH24_STENCIL8, Resolution.x, Resolution.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, DepthStencilBuffer);

        glBindRenderbuffer(GL_FRAMEBUFFER, 0);
    }
} // Rendering
