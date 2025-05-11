#pragma once
#include <glm/vec3.hpp>
#include <vector>

namespace Engine
{
    class Collider;
    class BoxCollider;
    class SphereCollider;
    class CapsuleCollider;
    class SpatialPartitioning;

    struct CollisionResult
    {
        bool hasCollision;
        glm::vec3 collisionPoint;
        glm::vec3 collisionNormal; // will be edited by physics

        CollisionResult() : hasCollision(false), collisionPoint(0.0f), collisionNormal(0.0f) {}
    };

    class ColliderVisitor
    {
    public:
        virtual ~ColliderVisitor() = default;

        virtual void ResolveCollisionBox(BoxCollider& box) = 0;
        virtual void ResolveCollisionSphere(SphereCollider& sphere) = 0;
        virtual void ResolveCollisionCapsule(CapsuleCollider& capsule) = 0;
    };

    class ConcreteColliderVisitor : public ColliderVisitor
    {
    private:
        bool collisionDetected;
        SpatialPartitioning* spatialPartitioning;

        CollisionResult CheckBoxBoxCollision(const BoxCollider& box1, const BoxCollider& box2);
        CollisionResult CheckBoxSphereCollision(const BoxCollider& box, const SphereCollider& sphere);
        CollisionResult CheckBoxCapsuleCollision(const BoxCollider& box, const CapsuleCollider& capsule);
        CollisionResult CheckSphereSphereCollision(const SphereCollider& sphere1, const SphereCollider& sphere2);
        CollisionResult CheckCapsuleSphereCollision(const CapsuleCollider& capsule, const SphereCollider& sphere);
        CollisionResult CheckCapsuleCapsuleCollision(const CapsuleCollider& capsule1, const CapsuleCollider& capsule2);

        glm::vec3 GetSeparationBoxBox(const BoxCollider& box1, const BoxCollider& box2);
        glm::vec3 GetSeparationBoxSphere(const BoxCollider& box, const SphereCollider& sphere);
        glm::vec3 GetSeparationBoxCapsule(const BoxCollider& box, const CapsuleCollider& capsule);
        glm::vec3 GetSeparationSphereSphere(const SphereCollider& sphere1, const SphereCollider& sphere2);
        glm::vec3 GetSeparationSphereCapsule(const SphereCollider& sphere, const CapsuleCollider& capsule);
        glm::vec3 GetSeparationCapsuleCapsule(const CapsuleCollider& capsule1, const CapsuleCollider& capsule2);

    protected:
        Collider* currentCollider;

    public:
        CollisionResult result;

        ConcreteColliderVisitor();

        void SetCurrentCollider(Collider* collider);

        void ResolveCollisionBox(BoxCollider& box) override;
        void ResolveCollisionSphere(SphereCollider& sphere) override;
        void ResolveCollisionCapsule(CapsuleCollider& capsule) override;

        void ManageCollisions();
    };

} // namespace Engine
