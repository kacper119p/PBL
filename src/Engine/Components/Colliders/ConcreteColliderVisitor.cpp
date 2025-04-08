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
            return !(box1.GetTransform()->GetPosition().x + box1.GetWidth() < box2.GetTransform()->GetPosition().x ||
                     box1.GetTransform()->GetPosition().x > box2.GetTransform()->GetPosition().x + box2.GetWidth() ||
                     box1.GetTransform()->GetPosition().y + box1.GetHeight() < box2.GetTransform()->GetPosition().y ||
                     box1.GetTransform()->GetPosition().y > box2.GetTransform()->GetPosition().y + box2.GetHeight() ||
                     box1.GetTransform()->GetPosition().z + box1.GetDepth() < box2.GetTransform()->GetPosition().z ||
                     box1.GetTransform()->GetPosition().z > box2.GetTransform()->GetPosition().z + box2.GetDepth());
        }

        bool CheckBoxSphereCollision(const BoxCollider& box, const SphereCollider& sphere)
        {
            glm::vec3 closestPoint = glm::clamp(sphere.GetTransform()->GetPosition(), box.GetTransform()->GetPosition(),
                                                box.GetTransform()->GetPosition() +
                                                        glm::vec3(box.GetWidth(), box.GetHeight(), box.GetDepth()));

            float distanceSquared = glm::distance2(closestPoint, sphere.GetTransform()->GetPosition());
            return distanceSquared <= (sphere.GetRadius() * sphere.GetRadius());
        }

        bool CheckBoxCapsuleCollision(const BoxCollider& box, const CapsuleCollider& capsule)
        {
            glm::vec3 boxMin = box.GetTransform()->GetPosition();
            glm::vec3 boxMax = boxMin + glm::vec3(box.GetWidth(), box.GetHeight(), box.GetDepth());

            glm::vec3 capsuleStart = capsule.GetTransform()->GetPosition(); 
            glm::vec3 capsuleEnd =
                    capsuleStart + glm::vec3(0.0f, capsule.GetHeight(), 0.0f); 
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
            // TODO: Implementacja kolizji między AABB a siatką
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
            // TODO: Implement collision checking between sphere and mesh
            return false;
        }

        bool CheckCapsuleCapsuleCollision(const CapsuleCollider& capsule1, const CapsuleCollider& capsule2)
        {
            glm::vec3 capsule1Start = capsule1.GetTransform()->GetPosition(); 
            glm::vec3 capsule1End =
                    capsule1Start + glm::vec3(0.0f, capsule1.GetHeight(), 0.0f);
            float capsule1Radius = capsule1.GetRadius();

            glm::vec3 capsule2Start = capsule2.GetTransform()->GetPosition();
            glm::vec3 capsule2End =
                    capsule2Start + glm::vec3(0.0f, capsule2.GetHeight(), 0.0f);
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
            // TODO: Implement collision checking between capsule and mesh
            return false;
        }

        bool CheckMeshMeshCollision(const MeshCollider& mesh1, const MeshCollider& mesh2)
        {
            // TODO: Implement collision checking between two meshes
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
