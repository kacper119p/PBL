#pragma once

#include "Collider.h"
#include "Models/Mesh.h" // TODO: Fix later. Double inclusion. I'm using this way because of indexing problem.
#include "Engine/Components/Renderers/Renderer.h"
namespace Engine
{
    /**
     * @brief Mesh collider class.
     * @details Represents a mesh-shaped collider.
     */
    class MeshCollider : public Collider
    {
    public:
        float _scale;
        Models::Mesh* _mesh_p;

        MeshCollider() = default;

        MeshCollider(Transform* transform, bool isTrigger = false, float scale = 1.0f, Models::Mesh* mesh_p = nullptr);

        virtual bool AcceptCollision(ColliderVisitor& visitor) override;
        //virtual bool CheckCollision(const Collider& other) override;

        inline virtual Collider* GetInstance() override { return this; }

        float GetScale() const;
        void SetScale(float scale);

        Models::Mesh* GetMesh() const;
        void SetMesh(Models::Mesh* mesh_p);

        MeshCollider& operator=(const MeshCollider& other);

        void DrawDebugMesh(const CameraRenderData& RenderData) override;

        void RenderDepth(const CameraRenderData& RenderData) override;

        void Render(const CameraRenderData& RenderData) override;

        void RenderDirectionalShadows(const CameraRenderData& RenderData) override;

        void RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                    const glm::mat4* SpaceTransformMatrices) override;

        void Start() override;
        void OnDestroy() override;

        #if EDITOR
        void DrawImGui() override {};
        #endif

        SERIALIZATION_EXPORT_CLASS(MeshCollider)
    };

} // namespace Engine
