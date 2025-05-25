#pragma once
#include <glm/vec3.hpp>
#include <vector>


namespace Engine
{
    class Collider;
    class BoxCollider;
    class SphereCollider;
    class CapsuleCollider;
    class MeshCollider;
    class SpatialPartitioning;
    class RigidBody;

    struct CollisionResult
    {
        bool hasCollision;
        glm::vec3 collisionPoint;
        glm::vec3 collisionNormal;
        float penetrationDepth;

        CollisionResult() :
            hasCollision(false), collisionPoint(0.0f), collisionNormal(0.0f), penetrationDepth(0.0f)
        {
        }
    };

    class ColliderVisitor
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

        void EmitCollision(Collider* Collider) const;

        void EmitTrigger(Collider* Collider) const;
        
    protected:
        Collider* currentCollider;

    public:
        CollisionResult result;

        ColliderVisitor();

        ColliderVisitor(Collider* collider);

        void SetCurrentCollider(Collider* collider);

        void ResolveCollisionBox(BoxCollider& box);

        void ResolveCollisionSphere(SphereCollider& sphere);

        void ResolveCollisionCapsule(CapsuleCollider& capsule);

        void ManageCollisions();
    };

} // namespace Engine
