#include "SceneFrameBuffer.h"

namespace Engine
{
    SceneFrameBuffer::SceneFrameBuffer(glm::ivec2 Resolution) :
        Resolution(Resolution)
    {
        glGenFramebuffers(1, &Id);
        UpdateBuffers();
    }

    SceneFrameBuffer::~SceneFrameBuffer()
    {
        glDeleteFramebuffers(1, &Id);
        glDeleteTextures(1, &ColorBuffer);
        glDeleteRenderbuffers(1, &DepthStencilBuffer);
    }

    void SceneFrameBuffer::UpdateBuffers()
    {
        glDeleteRenderbuffers(1, &DepthStencilBuffer);
        glGenRenderbuffers(1, &DepthStencilBuffer);
        glDeleteTextures(1, &ColorBuffer);
        glGenTextures(1, &ColorBuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, Id);

        glBindTexture(GL_TEXTURE_2D, ColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Resolution.x, Resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorBuffer, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, DepthStencilBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Resolution.x, Resolution.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, DepthStencilBuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} // Engine
