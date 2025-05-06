#pragma once
#include "Renderer.h"
#include "Engine/Rendering/CubeGeometry.h"
#include "Serialization/SerializedObject.h"

namespace Materials
{
    class Material;
}

namespace Engine
{
    class SkyboxRenderer final : public Renderer
    {
    private:
        Engine::CubeGeometry Cube;

    public:
        void RenderDepth(const CameraRenderData& RenderData) override;

        void Render(const CameraRenderData& RenderData) override;

        void RenderDirectionalShadows(const CameraRenderData& RenderData) override;

        void RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                    const glm::mat4* SpaceTransformMatrices) override;

#if EDITOR
        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_CLASS(SkyboxRenderer);
    };

} // Engine
