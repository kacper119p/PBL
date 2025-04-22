#pragma once
#include "Collider.h"
#include "Engine/Components/Renderers/Renderer.h"
namespace Engine
{
    /**
     * @brief Capsule collider class.
     * @details Represents a capsule-shaped collider.
     */
    class CapsuleCollider : public Collider
    {
    private:
        float _radius;
        float _height;

    public:
        CapsuleCollider() = default;
        CapsuleCollider(Transform* transform, bool isTrigger = false, float radius = 1.0f, float height = 2.0f);
        
        virtual bool AcceptCollision(ColliderVisitor& visitor) override;
        inline virtual Collider* GetInstance() override { return this; }

        float GetRadius() const;
        void SetRadius(float radius);

        float GetHeight() const;
        void SetHeight(float height);

        CapsuleCollider& operator=(const CapsuleCollider& other);

        void DrawDebugMesh() override;

        void RenderDepth(const CameraRenderData& RenderData) override;

        void Render(const CameraRenderData& RenderData) override;

        void RenderDirectionalShadows(const CameraRenderData& RenderData) override;

        void RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                    const glm::mat4* SpaceTransformMatrices) override;

        void Start() override;
        void OnDestroy() override;

        SERIALIZATION_EXPORT_CLASS(CapsuleCollider)
    };

} // namespace Engine
