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
        virtual void VisitBox(BoxCollider& box) = 0;
        virtual void VisitSphere(SphereCollider& sphere) = 0;
        virtual void VisitCapsule(CapsuleCollider& capsule) = 0;
        virtual void VisitMesh(MeshCollider& mesh) = 0;
    };

} // namespace Engine
