#include "RenderingManager.h"
#include "Engine/Exceptions/SingletonAlreadyExistsException.h"
#include "LightManager.h"

namespace Engine
{
    RenderingManager* RenderingManager::Instance = nullptr;

    RenderingManager::RenderingManager(const glm::ivec2 Resolution) :
        MultiSampledBuffer(Resolution)
    {
    }

    void RenderingManager::Initialize(const glm::ivec2 Resolution)
    {
        if (Instance)
        {
            throw SingletonAlreadyExistsException("Instance of RenderingManager already exists.");
        }
        Instance = new RenderingManager(Resolution);
    }

    void RenderingManager::RenderAll(const CameraRenderData& RenderData, int ScreenWidth, int ScreenHeight)
    {
        LightManager::GetInstance()->RenderShadowMaps(RenderData);

        MultiSampledBuffer.BindMultiSampled();

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

        if (Ui != nullptr)
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
            Ui->Render();
            glDisable(GL_BLEND);
        }

        MultiSampledBuffer.ResolveMultisampling();
        Bloom.Render(MultiSampledBuffer.GetResolvedColorBuffer());
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
    }
} // Engine
