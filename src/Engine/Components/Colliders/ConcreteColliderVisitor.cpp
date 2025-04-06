#pragma once

#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "ColliderVisitor.h"
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

    public:
        ConcreteColliderVisitor() : otherCollider(nullptr), collisionDetected(false) {}

        void SetOtherCollider(Collider* collider) { otherCollider = collider; }

        bool CheckCollision(Collider& collider)
        {
            if (collider.IsStatic() || otherCollider->IsStatic())
                return false;
            return collisionDetected;
            collider.AcceptCollision(*this);
            return collisionDetected;
        }

        void VisitBox(BoxCollider& box) override
        {
            // TODO: implement collision detection between currentCollider and box 
        }

        void VisitSphere(SphereCollider& sphere) override
        {
            // TODO: implement collision detection between currentCollider and sphere
        }

        void VisitCapsule(CapsuleCollider& capsule) override
        {
            // TODO: implement collision detection between currentCollider and capsule
        }

        void VisitMesh(MeshCollider& mesh) override
        {
            // TODO: implement collision detection between currentCollider and mesh
        }
    };
} // namespace Engine
