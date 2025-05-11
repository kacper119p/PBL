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
    private:
        unsigned int VAO = 0;
        unsigned int VBO = 0;
        unsigned int EBO = 0;

    public:

        float radius;

        SphereCollider();


        virtual bool AcceptCollision(ColliderVisitor& visitor) override;

        inline virtual Collider* GetInstance() override { return this; }

        float GetRadius() const;
        void SetRadius(float radius);

        SphereCollider& operator=(const SphereCollider& other);


        void DrawDebugMesh(const CameraRenderData& RenderData) override;

        void RenderDepth(const CameraRenderData& RenderData) override;

        void Render(const CameraRenderData& RenderData) override;

        void RenderDirectionalShadows(const CameraRenderData& RenderData) override;

        void RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                    const glm::mat4* SpaceTransformMatrices) override;

        SERIALIZATION_EXPORT_CLASS(SphereCollider)

        #if EDITOR
        void DrawImGui() override;
        #endif
    };

} // namespace Engine
