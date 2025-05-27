#pragma once
#include <glm/vec3.hpp>

#include "Engine/Textures/Texture.h"

#if EDITOR
#include "Engine/Components/Renderers/Renderer.h"
#else
#include "Engine/Components/Component.h"
#endif

namespace Engine
{

    class BloodSourceBase
#if EDITOR
            : public Renderer
#else
            : public Component
#endif
    {
    public:


    protected:
        Texture Texture;
        glm::vec3 Color = glm::vec3(0.0f, 0.0f, 0.0f);

    public:
        void Start() override;
#if EDITOR

    public:
        void RenderDepth(const CameraRenderData& RenderData) override;

        void Render(const CameraRenderData& RenderData) override;

        void RenderDirectionalShadows(const CameraRenderData& RenderData) override;

        void RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                    const glm::mat4* SpaceTransformMatrices) override;

    protected:
        void DrawImGui() override;
#endif
    };

}
