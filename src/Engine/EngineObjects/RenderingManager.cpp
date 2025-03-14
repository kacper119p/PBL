#include "RenderingManager.h"
#include "Engine/Exceptions/SingletonAlreadyExistsException.h"
#include "LightManager.h"

namespace Engine
{
    RenderingManager* RenderingManager::Instance = nullptr;

    RenderingManager::RenderingManager()
    {
        glGenFramebuffers(1, &SceneColorFrameBuffer);
        glGenRenderbuffers(1, &SceneColorRBO);
        glGenTextures(1, &SceneColorBuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, SceneColorFrameBuffer);

        glBindTexture(GL_TEXTURE_2D, SceneColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SceneColorBuffer, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, SceneColorRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, SceneColorRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderingManager::Initialize()
    {
        if (Instance)
        {
            throw SingletonAlreadyExistsException("Instance of RenderingManager already exists.");
        }
        Instance = new RenderingManager();
    }

    void RenderingManager::RenderAll(const CameraRenderData& RenderData, int ScreenWidth, int ScreenHeight)
    {
        LightManager::GetInstance()->RenderShadowMaps(RenderData);

        glBindFramebuffer(GL_FRAMEBUFFER, SceneColorFrameBuffer);

        glDepthMask(GL_TRUE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, ScreenWidth, ScreenHeight);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glCullFace(GL_BACK);

        for (Renderer* renderer : Renderers)
        {
            renderer->RenderDepth(RenderData);
        }

        glViewport(0, 0, ScreenWidth, ScreenHeight);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_FALSE);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glCullFace(GL_BACK);
        for (Renderer* renderer : Renderers)
        {
            renderer->Render(RenderData);
        }

        Bloom.Render(SceneColorBuffer);
    }

    void RenderingManager::RenderAllDirectionalShadowMap(const CameraRenderData& RenderData, unsigned int Target,
                                                         unsigned int Width, unsigned int Height)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, Target);
        glViewport(0, 0, Width, Height);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClear(GL_DEPTH_BUFFER_BIT);
        for (Renderer* renderer : Renderers)
        {
            renderer->RenderDirectionalShadows(RenderData);
        }
    }

    void
    RenderingManager::RenderAllPointSpotShadowMap(const glm::vec3& LightPosition, float LightRange,
                                                  glm::mat4* SpaceTransformMatrices,
                                                  unsigned int Target,
                                                  unsigned int Width, unsigned int Height)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, Target);
        glViewport(0, 0, Width, Height);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClear(GL_DEPTH_BUFFER_BIT);
        for (Renderer* renderer : Renderers)
        {
            renderer->RenderPointSpotShadows(LightPosition, LightRange, SpaceTransformMatrices);
        }
    }

    RenderingManager::~RenderingManager()
    {
        glDeleteFramebuffers(1, &SceneColorFrameBuffer);
        glDeleteRenderbuffers(1, &SceneColorRBO);
        glDeleteTextures(1, &SceneColorBuffer);
    }
} // Engine
