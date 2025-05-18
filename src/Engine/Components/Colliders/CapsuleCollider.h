#pragma once
#include "Collider.h"
#include "Engine/Components/Renderers/Renderer.h"

namespace Engine
{
    /**
     * @brief Capsule collider class.
     * @details Represents a capsule-shaped collider.
    */

    class CocreteColliderVisitor;

    class CapsuleCollider final : public Collider
    {
    private:
        float Radius;
        float Height;

#if EDITOR
        uint32_t Vao = 0;
        uint32_t Vbo = 0;
        uint32_t Ebo = 0;

        static constexpr uint8_t Segments = 16;
        static constexpr uint8_t Rings = 8;
#endif

    public:
        CapsuleCollider();

    public:
        ~CapsuleCollider() override;

        virtual bool AcceptCollision(ColliderVisitor& Visitor) override;

        inline virtual Collider* GetInstance() override { return this; }

        virtual glm::mat3 CalculateInertiaTensor(float mass) const override;

        glm::vec3 GetBoundingBox() const override
        {
            return glm::vec3(2.0f * Radius, Height + 2.0f * Radius, 2.0f * Radius);
        }

        float GetRadius() const
        {
            return Radius;
        }

        void SetRadius(float Radius)
        {
            this->Radius = Radius;
#if EDITOR
            UpdateBuffers();
#endif
        }

        float GetHeight() const
        {
            return Height;
        }

        void SetHeight(float Height)
        {
            this->Height -= Height;
#if EDITOR
            UpdateBuffers();
#endif
        }

        CapsuleCollider& operator=(const CapsuleCollider& other);

#if EDITOR
        void DrawDebugMesh(const CameraRenderData& RenderData);

        void RenderDepth(const CameraRenderData& RenderData) override;

        void Render(const CameraRenderData& RenderData) override;

        void RenderDirectionalShadows(const CameraRenderData& RenderData) override;

        void RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                    const glm::mat4* SpaceTransformMatrices) override;
#endif

    private:
#if EDITOR
        void UpdateBuffers();
#endif

        SERIALIZATION_EXPORT_CLASS(CapsuleCollider)

#if EDITOR
        void DrawImGui() override;
#endif
    };

} // namespace Engine
