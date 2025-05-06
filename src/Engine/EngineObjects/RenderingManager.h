#pragma once

#include <vector>
#include <Engine/Rendering/RenderersCollection.h>

#include "Engine/Components/Renderers/Renderer.h"
#include "CameraRenderData.h"
#include "Engine/Rendering/SceneFrameBuffer.h"
#include "Engine/Rendering/Ssao.h"
#include "Engine/Rendering/Postprocessing/BloomPostprocessingEffect.h"
#include "Engine/UI/Ui.h"
#include "Engine/Rendering/Frustum.h"

namespace Engine
{

    class RenderingManager final
    {
    private:
        static uint8_t MultisampleLevel;

        RenderersCollection Renderers;
        Ui::Ui* Ui = nullptr;

        static RenderingManager* Instance;

        SceneFrameBuffer MultiSampledBuffer;

        Ssao Ssao;
        BloomPostprocessingEffect Bloom;

        Frustum Frustum;

    private:
        explicit RenderingManager(glm::ivec2 Resolution);

    public:
        virtual ~RenderingManager();

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
            Renderers.AddRenderer(Renderer);
        }

        void UnregisterRenderer(const Renderer* const Renderer)
        {
            Renderers.RemoveRenderer(Renderer);
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

        void RenderAll(const CameraRenderData& RenderData, int ScreenWidth, int ScreenHeight);

        void RenderAllDirectionalShadowMap(const CameraRenderData& RenderData, unsigned int Target, unsigned int Width,
                                           unsigned int Height);

        void RenderAllPointSpotShadowMap(const glm::vec3& LightPosition, float LightRange,
                                         glm::mat4* SpaceTransformMatrices, unsigned int Target, unsigned int Width,
                                         unsigned int Height);
    };

} // Engine
