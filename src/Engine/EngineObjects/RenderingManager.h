#pragma once

#include <vector>
#include "Engine/Components/Renderers/Renderer.h"
#include "CameraRenderData.h"
#include "Engine/Rendering/Postprocessing/BloomPostprocessingEffect.h"

namespace Engine
{

    class RenderingManager
    {
    private:
        std::vector<Renderer*> Renderers;

        static RenderingManager* Instance;

        unsigned int SceneColorFrameBuffer;
        unsigned int SceneColorRBO;
        unsigned int SceneColorBuffer;

        BloomPostprocessingEffect Bloom;

    private:
        RenderingManager();

    public:
        virtual ~RenderingManager();

    public:
        static void Initialize();

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

        void RenderAll(const CameraRenderData& RenderData, int ScreenWidth, int ScreenHeight);

        void RenderAllDirectionalShadowMap(const CameraRenderData& RenderData, unsigned int Target, unsigned int Width,
                                           unsigned int Height);

        void RenderAllPointSpotShadowMap(const glm::vec3& LightPosition, float LightRange,
                                         glm::mat4* SpaceTransformMatrices, unsigned int Target, unsigned int Width,
                                         unsigned int Height);
    };

} // Engine
