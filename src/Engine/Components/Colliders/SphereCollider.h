#pragma once

#include "Collider.h"
#include "Engine/Components/Renderers/Renderer.h"
namespace Engine
{
    /**
     * @brief Sphere collider class.
     * @details Represents a sphere-shaped collider.
     */
    class SphereCollider : public Collider
    {

    public:
        float radius;

        SphereCollider() = default;

        SphereCollider(Transform* transform, bool isTrigger = false, float radius = 1.0f);

        virtual bool AcceptCollision(ColliderVisitor& visitor) override;
        //virtual bool CheckCollision(const Collider& other) override;

        inline virtual Collider* GetInstance() override { return this; }

        float GetRadius() const;
        void SetRadius(float radius);

        SphereCollider& operator=(const SphereCollider& other);

        void DrawDebugMesh() override;

        void RenderDepth(const CameraRenderData& RenderData) override;

        void Render(const CameraRenderData& RenderData) override;

        void RenderDirectionalShadows(const CameraRenderData& RenderData) override;

        void RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                    const glm::mat4* SpaceTransformMatrices) override;

        void Start() override;
        void OnDestroy() override;

        SERIALIZATION_EXPORT_CLASS(SphereCollider)
    };

} // namespace Engine
