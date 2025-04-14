#pragma once

#include <vector>
#include "Engine/Components/Renderers/Renderer.h"
#include "CameraRenderData.h"
#include "Engine/Rendering/SceneFrameBuffer.h"
#include "Engine/Rendering/Postprocessing/BloomPostprocessingEffect.h"
#include "Engine/UI/Ui.h"

namespace Engine
{

    class RenderingManager final
    {
    private:
        static uint8_t MultisampleLevel;

        std::vector<Renderer*> Renderers;
        Ui::Ui* Ui = nullptr;

        static RenderingManager* Instance;

        SceneFrameBuffer MultiSampledBuffer;

        BloomPostprocessingEffect Bloom;

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

        void RegisterRenderer(Renderer* Renderer)
        {
            Renderers.push_back(Renderer);
        }

        void UnregisterRenderer(Renderer* Renderer)
        {
            std::erase(Renderers, Renderer);
        }

        void RegisterUi(Ui::Ui* const Ui)
        {
            this->Ui = Ui;
        }

        void UnregisterUi(Ui::Ui* const Ui)
        {
            if (this->Ui == Ui)
            {
                this->Ui = nullptr;
            }
        }

        void RenderAll(const CameraRenderData& RenderData, int ScreenWidth, int ScreenHeight);

        void RenderAllDirectionalShadowMap(const CameraRenderData& RenderData, unsigned int Target, unsigned int Width,
                                           unsigned int Height);

        void RenderAllPointSpotShadowMap(const glm::vec3& LightPosition, float LightRange,
                                         glm::mat4* SpaceTransformMatrices, unsigned int Target, unsigned int Width,
                                         unsigned int Height);
    };

} // Engine
