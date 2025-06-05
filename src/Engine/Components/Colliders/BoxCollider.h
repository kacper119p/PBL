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

        glm::vec3 GetBoundingBox() const override { return glm::vec3(_width, _height, _depth); }

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
