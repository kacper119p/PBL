#pragma once
#include "Engine/Components/Renderers/Renderer.h"
#include "Models/Model.h"

namespace Engine
{

    class VacuumVfx final : public Renderer
    {
    private:
        static constexpr float NoiseSpeed = 0.25f;

    private:
        Models::Model* Model;

        uint64_t OuterTexture;
        uint64_t InnerTexture;

        float Time = 0.0;

        int32_t TimeLocation;
        int32_t TextureLocation;

        int32_t CameraLocationLocation;
        int32_t ViewMatrixLocation;
        int32_t ProjectionMatrixLocation;
        int32_t ObjectToWorldMatrixLocation;

        bool Active = false;

    public:
        VacuumVfx();

#if EDITOR
        void DrawImGui() override;
#endif

        void Start() override;

        void RenderDepth(const CameraRenderData& RenderData) override;

        void Render(const CameraRenderData& RenderData) override;

        void RenderDirectionalShadows(const CameraRenderData& RenderData) override;

        void RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                    const glm::mat4* SpaceTransformMatrices) override;

        void Activate()
        {
            Active = true;
        }

        void Deactivate()
        {
            Active = false;
        }

        SERIALIZATION_EXPORT_CLASS(VacuumVfx)
    };
}
