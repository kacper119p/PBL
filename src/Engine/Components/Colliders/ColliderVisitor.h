#pragma once

namespace Engine
{
    class BoxCollider;
    class SphereCollider;
    class CapsuleCollider;
    class MeshCollider;

    class ColliderVisitor
    {
    public:
        // according to visitor scheme this would be named VisitBox, VisitSphere etc.
        virtual void ResolveCollisionBox(BoxCollider& box) = 0;
        virtual void ResolveCollisionSphere(SphereCollider& sphere) = 0;
        virtual void ResolveCollisionCapsule(CapsuleCollider& capsule) = 0;
        virtual void ResolveCollisionMesh(MeshCollider& mesh) = 0;
    };

} // namespace Engine
