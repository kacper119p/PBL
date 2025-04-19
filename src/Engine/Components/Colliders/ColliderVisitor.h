#pragma once
#include <glm/vec3.hpp>

namespace Engine
{
    class Collider;
    class BoxCollider;
    class SphereCollider;
    class CapsuleCollider;
    class MeshCollider;
    class SpatialPartitioning;

    class ColliderVisitor
    {
    public:
        virtual ~ColliderVisitor() = default;

        virtual void ResolveCollisionBox(BoxCollider& box) = 0;
        virtual void ResolveCollisionSphere(SphereCollider& sphere) = 0;
        virtual void ResolveCollisionCapsule(CapsuleCollider& capsule) = 0;
        virtual void ResolveCollisionMesh(MeshCollider& mesh) = 0;
    };

    class ConcreteColliderVisitor : public ColliderVisitor
    {
    private:
        
        bool collisionDetected;
        SpatialPartitioning* spatialPartitioning;

        bool CheckBoxBoxCollision(const BoxCollider& box1, const BoxCollider& box2);
        bool CheckBoxSphereCollision(const BoxCollider& box, const SphereCollider& sphere);
        bool CheckBoxCapsuleCollision(const BoxCollider& box, const CapsuleCollider& capsule);
        bool CheckBoxMeshCollision(const BoxCollider& box, const MeshCollider& mesh);
        bool CheckSphereSphereCollision(const SphereCollider& sphere1, const SphereCollider& sphere2);
        bool CheckSphereMeshCollision(const SphereCollider& sphere, const MeshCollider& mesh);
        bool CheckCapsuleSphereCollision(const CapsuleCollider& capsule, const SphereCollider& sphere);
        bool CheckCapsuleCapsuleCollision(const CapsuleCollider& capsule1, const CapsuleCollider& capsule2);
        bool CheckCapsuleMeshCollision(const CapsuleCollider& capsule, const MeshCollider& mesh);
        bool CheckMeshMeshCollision(const MeshCollider& mesh1, const MeshCollider& mesh2);

        glm::vec3 GetSeparationBoxBox(const BoxCollider& box1, const BoxCollider& box2);

        glm::vec3 GetSeparationBoxSphere(const BoxCollider& box, const SphereCollider& sphere);

        glm::vec3 GetSeparationBoxCapsule(const BoxCollider& box, const CapsuleCollider& capsule);

        glm::vec3 GetSeparationSphereSphere(const SphereCollider& sphere1, const SphereCollider& sphere2);

        glm::vec3 GetSeparationSphereCapsule(const SphereCollider& sphere, const CapsuleCollider& capsule);

        glm::vec3 GetSeparationCapsuleCapsule(const CapsuleCollider& capsule1, const CapsuleCollider& capsule2);

    protected:
        Collider* currentCollider;

    public:
        ConcreteColliderVisitor();
        ConcreteColliderVisitor(SpatialPartitioning* partitioning, Collider* collider);

        static void SetCurrentCollider(Collider* collider);

        void ResolveCollisionBox(BoxCollider& box) override;
        void ResolveCollisionSphere(SphereCollider& sphere) override;
        void ResolveCollisionCapsule(CapsuleCollider& capsule) override;
        void ResolveCollisionMesh(MeshCollider& mesh) override;

        void ManageCollisions();
    };

} // namespace Engine
