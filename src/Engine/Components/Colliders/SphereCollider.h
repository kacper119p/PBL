#pragma once

#include "Collider.h"
#include "Engine/Components/Renderers/Renderer.h"

namespace Engine
{
    /**
     * @brief Sphere collider class.
     * @details Represents a sphere-shaped collider.
     */
    class SphereCollider final : public Collider
    {
    private:
#if EDITOR
        uint32_t Vao = 0;
        uint32_t Vbo = 0;
        uint32_t Ebo = 0;

        static constexpr uint8_t LatitudeSegments = 16;
        static constexpr uint8_t LongitudeSegments = 16;
#endif

    private:
        float radius;

    public:
        SphereCollider();

    public:
        ~SphereCollider() override;

    public:
        virtual bool AcceptCollision(ColliderVisitor& visitor) override;

        inline virtual Collider* GetInstance() override { return this; }

        float GetRadius() const
        {
            return radius;
        }

        void SetRadius(float radius)
        {
            this->radius = radius;
#if EDITOR
            UpdateBuffers();
#endif
        }

        SphereCollider& operator=(const SphereCollider& other);

#if EDITOR
        void DrawDebugMesh(const CameraRenderData& RenderData);

        void RenderDepth(const CameraRenderData& RenderData) override;

        void Render(const CameraRenderData& RenderData) override;

        void RenderDirectionalShadows(const CameraRenderData& RenderData) override;

        void RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                    const glm::mat4* SpaceTransformMatrices) override;
#endif
        SERIALIZATION_EXPORT_CLASS(SphereCollider)

#if EDITOR
        void DrawImGui() override;
#endif

    private:
#if EDITOR
        void UpdateBuffers();
#endif

    };

} // namespace Engine
