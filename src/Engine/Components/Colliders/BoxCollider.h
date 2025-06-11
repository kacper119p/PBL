#pragma once

#include "Collider.h"
#include "Engine/Components/Renderers/Renderer.h"

namespace Engine
{
    /**
     * @brief Box collider class.
     * @details Represents a box-shaped collider.
    */

    class ColliderVisitor;

    class BoxCollider final : public Collider
    {
    private:
        float _width;
        float _height;
        float _depth;

#if EDITOR
        uint32_t Vao = 0;
        uint32_t Vbo = 0;
        uint32_t Ebo = 0;
#endif

    public:
        BoxCollider();

    public:
        ~BoxCollider() override;

    public:
        bool AcceptCollision(ColliderVisitor& visitor) override;

        //virtual bool CheckCollision(const Collider& other) override;

        inline Collider* GetInstance() override { return this; }

        ColliderAABB GetBoundingBox() const override
        {
            glm::mat4 transform = GetTransform()->GetLocalToWorldMatrix();
            glm::vec3 halfExtents = glm::vec3(_width, _height, _depth) * 0.5f;

            glm::vec3 localVertices[8] = {
                    {-halfExtents.x, -halfExtents.y, -halfExtents.z}, {halfExtents.x, -halfExtents.y, -halfExtents.z},
                    {-halfExtents.x, halfExtents.y, -halfExtents.z},  {halfExtents.x, halfExtents.y, -halfExtents.z},
                    {-halfExtents.x, -halfExtents.y, halfExtents.z},  {halfExtents.x, -halfExtents.y, halfExtents.z},
                    {-halfExtents.x, halfExtents.y, halfExtents.z},   {halfExtents.x, halfExtents.y, halfExtents.z},
            };

            glm::vec3 min(FLT_MAX);
            glm::vec3 max(-FLT_MAX);

            for (const auto& local : localVertices)
            {
                glm::vec3 world = glm::vec3(transform * glm::vec4(local, 1.0f));
                min = glm::min(min, world);
                max = glm::max(max, world);
            }

            ColliderAABB aabb;
            aabb.min = min;
            aabb.max = max;
            return aabb;
        }


        float GetWidth() const
        {
            return _width;
        }

        void SetWidth(float width)
        {
            _width = width;
#if EDITOR
            UpdateBuffers();
#endif
        }

        float GetHeight() const
        {
            return _height;
        }

        void SetHeight(float height)
        {
            _height = height;
#if EDITOR
            UpdateBuffers();
#endif
        }

        float GetDepth() const
        {
            return _depth;
        }

        void SetDepth(float depth)
        {
            _depth = depth;
#if EDITOR
            UpdateBuffers();
#endif
        }

        glm::mat3 CalculateInertiaTensorBody(float mass) const override;

        PrimitiveMesh* GetMesh();
        

        void Start() override;
#if EDITOR
        void RenderDepth(const CameraRenderData& RenderData) override;

        void Render(const CameraRenderData& RenderData) override;

        void RenderDirectionalShadows(const CameraRenderData& RenderData) override;

        void RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                    const glm::mat4* SpaceTransformMatrices) override;

        void DrawDebugMesh(const CameraRenderData& RenderData, Shaders::Shader Shader) const;
#endif
        BoxCollider& operator=(const BoxCollider& other);

#if EDITOR
        void DrawImGui() override;
#endif

    private:
#if EDITOR
        void UpdateBuffers();
#endif

        SERIALIZATION_EXPORT_CLASS(BoxCollider)

    };

} // namespace Engine
