#pragma once

#if EDITOR
#include "Engine/Components/Renderers/Renderer.h"
#else
#include "Engine/Components/Component.h"
#endif
#include "Engine/Textures/Texture.h"


namespace Engine
{
    class BloodEraser
#if EDITOR
            : public Renderer
#else
            : public Component
#endif
    {
    private:
        Texture Texture;

    public:
        BloodEraser();

    public:
        ~BloodEraser() override;

        void Start() override;

#if EDITOR

    public:
        void RenderDepth(const CameraRenderData& RenderData) override;

        void Render(const CameraRenderData& RenderData) override;

        void RenderDirectionalShadows(const CameraRenderData& RenderData) override;

        void RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                    const glm::mat4* SpaceTransformMatrices) override;

        void DrawImGui() override;
#endif

    public:
        void Draw() const;

        SERIALIZATION_EXPORT_CLASS(BloodEraser)
    };

}
