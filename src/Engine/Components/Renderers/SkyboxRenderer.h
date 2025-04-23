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
        Materials::Material* Material = nullptr;

    public:
        [[nodiscard]] Materials::Material* GetMaterial() const
        {
            return Material;
        }

        void SetMaterial(Materials::Material* const Material)
        {
            this->Material = Material;
        }

        void RenderDepth(const CameraRenderData& RenderData) override;

        void Render(const CameraRenderData& RenderData) override;

        void RenderDirectionalShadows(const CameraRenderData& RenderData) override;

        void RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                    const glm::mat4* SpaceTransformMatrices) override;

        SERIALIZATION_EXPORT_CLASS(SkyboxRenderer);
    };

} // Engine
