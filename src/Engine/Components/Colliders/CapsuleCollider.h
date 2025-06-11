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

        glm::mat3 CalculateInertiaTensorBody(float mass) const override;

        PrimitiveMesh* GetMesh();

        ColliderAABB GetBoundingBox() const override
        {
            glm::mat4 transform = GetTransform()->GetLocalToWorldMatrix();
            glm::vec3 center = glm::vec3(transform[3]);

            float halfHeight = Height * 0.5f;
            float r = Radius;

            glm::vec3 capsuleAxis = glm::normalize(glm::vec3(transform[1]));

            glm::vec3 extentAlongAxis = capsuleAxis * halfHeight;

            glm::vec3 axisX, axisZ;
            if (glm::abs(glm::dot(capsuleAxis, glm::vec3(0, 1, 0))) < 0.99f)
                axisX = glm::normalize(glm::cross(capsuleAxis, glm::vec3(0, 1, 0)));
            else
                axisX = glm::normalize(glm::cross(capsuleAxis, glm::vec3(1, 0, 0)));
            axisZ = glm::cross(capsuleAxis, axisX);

            glm::vec3 radiusX = axisX * r;
            glm::vec3 radiusZ = axisZ * r;

            glm::vec3 candidates[] = {
                    center + extentAlongAxis + radiusX + radiusZ, center + extentAlongAxis + radiusX - radiusZ,
                    center + extentAlongAxis - radiusX + radiusZ, center + extentAlongAxis - radiusX - radiusZ,
                    center - extentAlongAxis + radiusX + radiusZ, center - extentAlongAxis + radiusX - radiusZ,
                    center - extentAlongAxis - radiusX + radiusZ, center - extentAlongAxis - radiusX - radiusZ,
            };

            glm::vec3 min(FLT_MAX);
            glm::vec3 max(-FLT_MAX);

            for (const auto& p : candidates)
            {
                min = glm::min(min, p);
                max = glm::max(max, p);
            }

            ColliderAABB aabb;
            aabb.min = min;
            aabb.max = max;

            return aabb;
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
        void DrawDebugMesh(const CameraRenderData& RenderData, Shaders::Shader Shader);

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
