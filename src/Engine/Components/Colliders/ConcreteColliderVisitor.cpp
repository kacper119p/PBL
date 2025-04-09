#pragma once

#include "ColliderVisitor.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "MeshCollider.h"
#include "SphereCollider.h"

namespace Engine
{
    /**
     * @brief Concrete implementation of ColliderVisitor for collision detection.
     *        This class implements the visitor pattern to handle collision detection. 
     *        This should probably be treated as a singleton (?).
     *        I'll elaborate later when I grasp the ✨IDEA✨ (got problems now).
     */
    class ConcreteColliderVisitor : public ColliderVisitor
    {
    private:
        Collider* currentCollider;
        bool collisionDetected;

        bool CheckBoxBoxCollision(const BoxCollider& box1, const BoxCollider& box2)
        {
            const glm::mat4& transform1 = box1.GetTransform()->GetLocalToWorldMatrix();
            const glm::mat4& transform2 = box2.GetTransform()->GetLocalToWorldMatrix();

            glm::vec3 box1Min = transform1 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec3 box1Max = transform1 * glm::vec4(box1.GetWidth(), box1.GetHeight(), box1.GetDepth(), 1.0f);

            glm::vec3 box2Min = transform2 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec3 box2Max = transform2 * glm::vec4(box2.GetWidth(), box2.GetHeight(), box2.GetDepth(), 1.0f);

            return !(box1Max.x < box2Min.x || box1Min.x > box2Max.x || box1Max.y < box2Min.y || box1Min.y > box2Max.y ||
                     box1Max.z < box2Min.z || box1Min.z > box2Max.z);
        }

        bool CheckBoxSphereCollision(const BoxCollider& box, const SphereCollider& sphere)
        {
            const glm::mat4& boxTransform = box.GetTransform()->GetLocalToWorldMatrix();
            const glm::mat4& sphereTransform = sphere.GetTransform()->GetLocalToWorldMatrix();

            glm::vec3 sphereCenter = sphereTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

            glm::vec3 boxCenter = boxTransform *
                                  glm::vec4(box.GetWidth() / 2.0f, box.GetHeight() / 2.0f, box.GetDepth() / 2.0f, 1.0f);
            glm::vec3 boxHalfExtents = glm::vec3(box.GetWidth() / 2.0f, box.GetHeight() / 2.0f, box.GetDepth() / 2.0f);

            glm::vec3 xAxis = glm::normalize(glm::vec3(boxTransform[0])); 
            glm::vec3 yAxis = glm::normalize(glm::vec3(boxTransform[1])); 
            glm::vec3 zAxis = glm::normalize(glm::vec3(boxTransform[2])); 

            glm::vec3 delta = sphereCenter - boxCenter;

            glm::vec3 closestPoint = boxCenter;

            float distanceX = glm::dot(delta, xAxis);
            float distanceY = glm::dot(delta, yAxis);
            float distanceZ = glm::dot(delta, zAxis);

            distanceX = glm::clamp(distanceX, -boxHalfExtents.x, boxHalfExtents.x);
            distanceY = glm::clamp(distanceY, -boxHalfExtents.y, boxHalfExtents.y);
            distanceZ = glm::clamp(distanceZ, -boxHalfExtents.z, boxHalfExtents.z);

            closestPoint += distanceX * xAxis;
            closestPoint += distanceY * yAxis;
            closestPoint += distanceZ * zAxis;

            float distanceSquared = glm::distance2(closestPoint, sphereCenter);

            return distanceSquared <= (sphere.GetRadius() * sphere.GetRadius());
        }

        bool CheckBoxCapsuleCollision(const BoxCollider& box, const CapsuleCollider& capsule)
        {
            const glm::mat4& boxTransform = box.GetTransform()->GetLocalToWorldMatrix();
            const glm::mat4& capsuleTransform = capsule.GetTransform()->GetLocalToWorldMatrix();

            glm::vec3 boxMin = boxTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec3 boxMax = boxTransform * glm::vec4(box.GetWidth(), box.GetHeight(), box.GetDepth(), 1.0f);

            glm::vec3 capsuleStart = capsuleTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec3 capsuleEnd = capsuleTransform * glm::vec4(0.0f, capsule.GetHeight(), 0.0f, 1.0f);
            float capsuleRadius = capsule.GetRadius();

            glm::vec3 closestPointOnAABB = glm::clamp(capsuleStart, boxMin, boxMax);

            glm::vec3 capsuleAxis = capsuleEnd - capsuleStart;
            float capsuleAxisLengthSquared = glm::dot(capsuleAxis, capsuleAxis);

            if (capsuleAxisLengthSquared > 0.0f)
            {
                float t = glm::dot(closestPointOnAABB - capsuleStart, capsuleAxis) / capsuleAxisLengthSquared;
                t = glm::clamp(t, 0.0f, 1.0f);
                glm::vec3 closestPointOnCapsule = capsuleStart + t * capsuleAxis;

                float distanceSquared = glm::distance2(closestPointOnCapsule, closestPointOnAABB);
                return distanceSquared <= (capsuleRadius * capsuleRadius);
            }

            float distanceSquared = glm::distance2(capsuleStart, closestPointOnAABB);
            return distanceSquared <= (capsuleRadius * capsuleRadius);
        }

        bool CheckBoxMeshCollision(const BoxCollider& box, const MeshCollider& mesh)
        {
            const glm::mat4& boxTransform = box.GetTransform()->GetLocalToWorldMatrix();
            const glm::mat4& meshTransform = mesh.GetTransform()->GetLocalToWorldMatrix();

            glm::vec3 boxMin = boxTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec3 boxMax = boxTransform * glm::vec4(box.GetWidth(), box.GetHeight(), box.GetDepth(), 1.0f);

            const auto& vertices = mesh.GetMesh()->GetVertices();

            for (const auto& vertex : vertices)
            {
                glm::vec3 worldVertex = meshTransform * glm::vec4(vertex, 1.0f);
                if (worldVertex.x >= boxMin.x && worldVertex.x <= boxMax.x && worldVertex.y >= boxMin.y &&
                    worldVertex.y <= boxMax.y && worldVertex.z >= boxMin.z && worldVertex.z <= boxMax.z)
                {
                    return true;
                }
            }

            return false;
        }

        bool CheckSphereSphereCollision(const SphereCollider& sphere1, const SphereCollider& sphere2)
        {
            float distance =
                    glm::distance(sphere1.GetTransform()->GetPosition(), sphere2.GetTransform()->GetPosition());
            return distance <= (sphere1.GetRadius() + sphere2.GetRadius());
        }

        bool CheckSphereMeshCollision(const SphereCollider& sphere, const MeshCollider& mesh)
        {
            const glm::mat4& sphereTransform = sphere.GetTransform()->GetLocalToWorldMatrix();
            const glm::mat4& meshTransform = mesh.GetTransform()->GetLocalToWorldMatrix();

            glm::vec3 sphereCenter = sphereTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            float sphereRadius = sphere.GetRadius();

            const auto& vertices = mesh.GetMesh()->GetVertices();

            for (const auto& vertex : vertices)
            {
                glm::vec3 worldVertex = meshTransform * glm::vec4(vertex, 1.0f);
                float distanceSquared = glm::distance2(sphereCenter, worldVertex);
                if (distanceSquared <= (sphereRadius * sphereRadius))
                {
                    return true;
                }
            }

            return false;
        }

        bool CheckCapsuleCapsuleCollision(const CapsuleCollider& capsule1, const CapsuleCollider& capsule2)
        {
            const glm::mat4& transform1 = capsule1.GetTransform()->GetLocalToWorldMatrix();
            const glm::mat4& transform2 = capsule2.GetTransform()->GetLocalToWorldMatrix();

            glm::vec3 capsule1Start = transform1 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec3 capsule1End = transform1 * glm::vec4(0.0f, capsule1.GetHeight(), 0.0f, 1.0f);
            float capsule1Radius = capsule1.GetRadius();

            glm::vec3 capsule2Start = transform2 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec3 capsule2End = transform2 * glm::vec4(0.0f, capsule2.GetHeight(), 0.0f, 1.0f);
            float capsule2Radius = capsule2.GetRadius();

            glm::vec3 u = capsule1End - capsule1Start;
            glm::vec3 v = capsule2End - capsule2Start;
            glm::vec3 w = capsule1Start - capsule2Start;

            float a = glm::dot(u, u);
            float b = glm::dot(u, v);
            float c = glm::dot(v, v);
            float d = glm::dot(u, w);
            float e = glm::dot(v, w);

            float denominator = a * c - b * b;

            float s, t;
            if (denominator > 1e-6f)
            {
                s = (b * e - c * d) / denominator;
                t = (a * e - b * d) / denominator;
            }
            else
            {
                s = 0.0f;
                t = (e > 0.0f ? 1.0f : 0.0f);
            }

            s = glm::clamp(s, 0.0f, 1.0f);
            t = glm::clamp(t, 0.0f, 1.0f);

            glm::vec3 closestPointOnCapsule1 = capsule1Start + s * u;
            glm::vec3 closestPointOnCapsule2 = capsule2Start + t * v;

            float distanceSquared = glm::distance2(closestPointOnCapsule1, closestPointOnCapsule2);

            float combinedRadius = capsule1Radius + capsule2Radius;
            return distanceSquared <= (combinedRadius * combinedRadius);
        }

        bool CheckCapsuleMeshCollision(const CapsuleCollider& capsule, const MeshCollider& mesh)
        {
            const glm::mat4& capsuleTransform = capsule.GetTransform()->GetLocalToWorldMatrix();
            const glm::mat4& meshTransform = mesh.GetTransform()->GetLocalToWorldMatrix();

            glm::vec3 capsuleStart = capsuleTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec3 capsuleEnd = capsuleTransform * glm::vec4(0.0f, capsule.GetHeight(), 0.0f, 1.0f);
            float capsuleRadius = capsule.GetRadius();

            const auto& vertices = mesh.GetMesh()->GetVertices();

            for (const auto& vertex : vertices)
            {
                glm::vec3 worldVertex = meshTransform * glm::vec4(vertex, 1.0f);

                glm::vec3 capsuleAxis = capsuleEnd - capsuleStart;
                float capsuleAxisLengthSquared = glm::dot(capsuleAxis, capsuleAxis);

                float t = glm::dot(worldVertex - capsuleStart, capsuleAxis) / capsuleAxisLengthSquared;
                t = glm::clamp(t, 0.0f, 1.0f);
                glm::vec3 closestPointOnCapsule = capsuleStart + t * capsuleAxis;

                float distanceSquared = glm::distance2(closestPointOnCapsule, worldVertex);
                if (distanceSquared <= (capsuleRadius * capsuleRadius))
                {
                    return true;
                }
            }

            return false;
        }

        bool CheckMeshMeshCollision(const MeshCollider& mesh1, const MeshCollider& mesh2)
        {
            const glm::mat4& transform1 = mesh1.GetTransform()->GetLocalToWorldMatrix();
            const glm::mat4& transform2 = mesh2.GetTransform()->GetLocalToWorldMatrix();

            const auto& vertices1 = mesh1.GetMesh()->GetVertices();
            const auto& vertices2 = mesh2.GetMesh()->GetVertices();

            for (const auto& vertex1 : vertices1)
            {
                glm::vec3 worldVertex1 = transform1 * glm::vec4(vertex1, 1.0f);

                for (const auto& vertex2 : vertices2)
                {
                    glm::vec3 worldVertex2 = transform2 * glm::vec4(vertex2, 1.0f);

                    if (glm::distance2(worldVertex1, worldVertex2) < 1e-6f)
                    {
                        return true;
                    }
                }
            }

            return false;
        }

    public:
        ConcreteColliderVisitor() : currentCollider(nullptr), collisionDetected(false) {}

        void SetCurrentCollider(Collider* collider) { currentCollider = collider; }

        void ResolveCollisionBox(BoxCollider& box) override
        {
            if (!currentCollider)
                return;

            switch (currentCollider->colliderType)
            {
                case BOX:
                {
                    auto& thisBox = static_cast<BoxCollider&>(*currentCollider);
                    collisionDetected = CheckBoxBoxCollision(box, thisBox);
                    break;
                }
                case SPHERE:
                {
                    auto& sphere = static_cast<SphereCollider&>(*currentCollider);
                    collisionDetected = CheckBoxSphereCollision(box, sphere);
                    break;
                }
                case CAPSULE:
                {
                    auto& capsule = static_cast<CapsuleCollider&>(*currentCollider);
                    collisionDetected = CheckBoxCapsuleCollision(box, capsule);
                    break;
                }
                case MESH:
                {
                    auto& mesh = static_cast<MeshCollider&>(*currentCollider);
                    collisionDetected = CheckBoxMeshCollision(box, mesh);
                    break;
                }
            }
        }

        void ResolveCollisionSphere(SphereCollider& sphere) override
        {
            if (!currentCollider)
                return;

            switch (currentCollider->colliderType)
            {
                case BOX:
                {
                    auto& box = static_cast<BoxCollider&>(*currentCollider);
                    collisionDetected = CheckBoxSphereCollision(box, sphere);
                    break;
                }
                case SPHERE:
                {
                    auto& otherSphere = static_cast<SphereCollider&>(*currentCollider);
                    collisionDetected = CheckSphereSphereCollision(sphere, otherSphere);
                    break;
                }
                case CAPSULE:
                {
                    auto& capsule = static_cast<CapsuleCollider&>(*currentCollider);
                    collisionDetected = CheckSphereCapsuleCollision(sphere, capsule);
                    break;
                }
                case MESH:
                {
                    auto& mesh = static_cast<MeshCollider&>(*currentCollider);
                    collisionDetected = CheckSphereMeshCollision(sphere, mesh);
                    break;
                }
            }
        }

        void ResolveCollisionCapsule(CapsuleCollider& capsule) override
        {
            if (!currentCollider)
                return;

            switch (currentCollider->colliderType)
            {
                case BOX:
                {
                    auto& box = static_cast<BoxCollider&>(*currentCollider);
                    collisionDetected = CheckBoxCapsuleCollision(box, capsule);
                    break;
                }
                case SPHERE:
                {
                    auto& sphere = static_cast<SphereCollider&>(*currentCollider);
                    collisionDetected = CheckSphereCapsuleCollision(sphere, capsule);
                    break;
                }
                case CAPSULE:
                {
                    auto& otherCapsule = static_cast<CapsuleCollider&>(*currentCollider);
                    collisionDetected = CheckCapsuleCapsuleCollision(capsule, otherCapsule);
                    break;
                }
                case MESH:
                {
                    auto& mesh = static_cast<MeshCollider&>(*currentCollider);
                    collisionDetected = CheckCapsuleMeshCollision(capsule, mesh);
                    break;
                }
            }
        }

        void ResolveCollisionMesh(MeshCollider& mesh) override
        {
            if (!currentCollider)
                return;

            switch (currentCollider->colliderType)
            {
                case BOX:
                {
                    auto& box = static_cast<BoxCollider&>(*currentCollider);
                    collisionDetected = CheckBoxMeshCollision(box, mesh);
                    break;
                }
                case SPHERE:
                {
                    auto& sphere = static_cast<SphereCollider&>(*currentCollider);
                    collisionDetected = CheckSphereMeshCollision(sphere, mesh);
                    break;
                }
                case CAPSULE:
                {
                    auto& capsule = static_cast<CapsuleCollider&>(*currentCollider);
                    collisionDetected = CheckCapsuleMeshCollision(capsule, mesh);
                    break;
                }
                case MESH:
                {
                    auto& otherMesh = static_cast<MeshCollider&>(*currentCollider);
                    collisionDetected = CheckMeshMeshCollision(mesh, otherMesh);
                    break;
                }
            }
        }
    };
} // namespace Engine
