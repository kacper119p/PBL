#pragma once
#include "Engine/Components/Interfaces/IUpdateable.h"
#include "Engine/Components/Renderers/Renderer.h"
#include "Models/Model.h"

namespace Engine
{

    class VacuumVfx final : public Renderer, public IUpdateable
    {
    private:
        static constexpr float NoiseSpeed = 0.25f;
        static constexpr float TransitionSpeed = 1.0f / 0.2f;

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
        int32_t VisibilityLocation;

        bool Active = false;

    public:
        VacuumVfx();

    public:
        ~VacuumVfx() override;

    public:
#if EDITOR
        void DrawImGui() override;
#endif

        void Start() override;

        void Update(float DeltaTime) override;

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

        void DeactivateImmediate()
        {
            Active = false;
            Time = 0.0f;
        }

        SERIALIZATION_EXPORT_CLASS(VacuumVfx)
    };
}
