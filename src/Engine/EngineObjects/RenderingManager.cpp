#include "RenderingManager.h"
#include "Engine/Exceptions/SingletonAlreadyExistsException.h"
#include "LightManager.h"
#include "Materials/Material.h"

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

    void RenderingManager::RenderAll(const CameraRenderData& RenderData, const int ScreenWidth, const int ScreenHeight,
                                     const float DeltaTime)
    {
        Frustum.UpdateFrustum(RenderData);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        for (const auto& renderersGroup : ParticleEmitters)
        {
            for (ParticleEmitter* const renderer : renderersGroup.Renderers)
            {
                renderer->DispatchSpawnShaders(DeltaTime);
            }
        }

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        for (const auto& renderersGroup : ParticleEmitters)
        {
            for (ParticleEmitter* const renderer : renderersGroup.Renderers)
            {
                renderer->DispatchUpdateShaders(DeltaTime);
            }
        }

        LightManager::GetInstance()->RenderShadowMaps(RenderData);

        MultiSampledBuffer.BindMultiSampled();
        MultiSampledBuffer.WriteNormals();

        glDepthMask(GL_TRUE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, ScreenWidth, ScreenHeight);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glCullFace(GL_BACK);

        for (const auto& renderersGroup : Renderers)
        {
            renderersGroup.Material->UseDepthPass();
            for (Renderer* const renderer : renderersGroup.Renderers)
            {
                renderer->RenderDepth(RenderData);
            }
        }

        MultiSampledBuffer.ResolveNormals();
        Ssao.Render(RenderData, MultiSampledBuffer.GetResolvedNormals());

        MultiSampledBuffer.BindMultiSampled();
        MultiSampledBuffer.WriteOcclusion();

        glViewport(0, 0, ScreenWidth, ScreenHeight);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_FALSE);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glCullFace(GL_BACK);
        LightManager::GetInstance()->SetupLightsForRendering(RenderData);
        for (const auto& renderersGroup : Renderers)
        {
            renderersGroup.Material->Use();
            for (Renderer* const renderer : renderersGroup.Renderers)
            {
                renderer->Render(RenderData);
            }
        }

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        for (const auto& renderersGroup : TransparentRenderers)
        {
            renderersGroup.Material->Use();
            for (Renderer* const renderer : renderersGroup.Renderers)
            {
                renderer->Render(RenderData);
            }
        }

        MultiSampledBuffer.ResolveMultisampling();
        MultiSampledBuffer.BindResolved();

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glEnable(GL_BLEND);
        for (const auto& renderersGroup : ParticleEmitters)
        {
            renderersGroup.Material->Use();
            for (ParticleEmitter* const renderer : renderersGroup.Renderers)
            {
                renderer->Render(RenderData);
            }
        }
        glDisable(GL_BLEND);

        if (Ui != nullptr)
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
            Ui->Render();
            glDisable(GL_BLEND);
        }

        GodRays.Render(MultiSampledBuffer);
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
        for (const auto& renderersGroup : Renderers)
        {
            renderersGroup.Material->UseDirectionalShadows();
            for (Renderer* const renderer : renderersGroup.Renderers)
            {
                renderer->RenderDirectionalShadows(RenderData);
            }
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
        for (const auto& renderersGroup : Renderers)
        {
            renderersGroup.Material->UsePointSpotShadows();
            for (Renderer* const renderer : renderersGroup.Renderers)
            {
                renderer->RenderPointSpotShadows(LightPosition, LightRange, SpaceTransformMatrices);
            }
        }
    }

    RenderingManager::~RenderingManager()
    {
    }
} // Engine
