#pragma once

#include <Engine/Rendering/RenderersCollection.h>

#include "Engine/Components/Renderers/Renderer.h"
#include "CameraRenderData.h"
#include "Engine/Components/Renderers/OutlinedModelRenderer.h"
#include "Engine/Rendering/SceneFrameBuffer.h"
#include "Engine/Rendering/Ssao.h"
#include "Engine/Rendering/Postprocessing/BloomPostprocessingEffect.h"
#include "Engine/UI/Ui.h"
#include "Engine/Rendering/Frustum.h"
#include "Engine/Rendering/Postprocessing/GodRays.h"
#include "Materials/Material.h"

namespace Engine
{

    class RenderingManager final
    {
    private:
        static uint8_t MultisampleLevel;

        RenderersCollection Renderers;
        std::vector<OutlinedModelRenderer*> OutlinedRenderers;
        RenderersCollection TransparentRenderers;
        ParticleEmittersCollection ParticleEmitters;
        Ui::Ui* Ui = nullptr;

        static RenderingManager* Instance;

        SceneFrameBuffer MultiSampledBuffer;

        Ssao Ssao;
        BloomPostprocessingEffect Bloom;
        GodRays GodRays;

        Frustum Frustum;

        const glm::ivec2 Resolution;

    private:
        explicit RenderingManager(glm::ivec2 Resolution);

    public:
        ~RenderingManager();

    public:
        static void Initialize(glm::ivec2 Resolution);

    public:
        RenderingManager(RenderingManager const&) = delete;

        void operator=(RenderingManager const&) = delete;

    public:
        static RenderingManager* GetInstance()
        {
            return Instance;
        }

        void RegisterRenderer(Renderer* const Renderer)
        {
            if (Renderer->GetMaterial()->IsTransparent())
            {
                TransparentRenderers.AddRenderer(Renderer);
            }
            else
            {
                Renderers.AddRenderer(Renderer);
            }
        }

        void UnregisterRenderer(const Renderer* const Renderer)
        {
            if (Renderer->GetMaterial()->IsTransparent())
            {
                TransparentRenderers.RemoveRenderer(Renderer);
            }
            else
            {
                Renderers.RemoveRenderer(Renderer);
            }
        }

        void RegisterOutlinedRenderer(OutlinedModelRenderer* const Renderer)
        {
            OutlinedRenderers.push_back(Renderer);
        }

        void UnregisterOutlinedRenderer(const OutlinedModelRenderer* const Renderer)
        {
            const auto iterator = std::ranges::find(OutlinedRenderers, Renderer);
            if (iterator != OutlinedRenderers.end())
            {
                std::erase(OutlinedRenderers, Renderer);
            }
        }

        void RegisterParticleEmitter(ParticleEmitter* const Renderer)
        {
            ParticleEmitters.AddRenderer(Renderer);
        }

        void UnregisterParticleEmitter(const ParticleEmitter* const Renderer)
        {
            ParticleEmitters.RemoveRenderer(Renderer);
        }

        void RegisterUi(Ui::Ui* const Ui)
        {
            this->Ui = Ui;
        }

        void UnregisterUi(const Ui::Ui* const Ui)
        {
            if (this->Ui == Ui)
            {
                this->Ui = nullptr;
            }
        }

        [[nodiscard]] unsigned int GetSsaoTextureId() const
        {
            return Ssao.GetColorTexture();
        }

    public:
        [[nodiscard]] const class Frustum& GetFrustum() const
        {
            return Frustum;
        }

        void RenderAll(const CameraRenderData& RenderData, float DeltaTime);

        void RenderAllDirectionalShadowMap(const CameraRenderData& RenderData, unsigned int Target, unsigned int Width,
                                           unsigned int Height);

        void RenderAllPointSpotShadowMap(const glm::vec3& LightPosition, float LightRange,
                                         const glm::mat4* SpaceTransformMatrices, unsigned int Target,
                                         unsigned int Width,
                                         unsigned int Height);
    };

} // Engine
