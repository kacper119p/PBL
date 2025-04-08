#pragma once

#include "../Component.h" // TODO: Fix later. I'm using this way because of indexing problem.
#include "../Transform.h" // TODO: Fix later. I'm using this way because of indexing problem.

#include "Events/TAction.h"
#include "Events/TEvent.h"
#include "ColliderVisitor.h"
namespace Engine
{
    enum ColliderTypeE
    {
        BOX,
        SPHERE,
        CAPSULE,
        MESH
    };
    /*
     * @brief Base class for all colliders. Subtypes: boxCollider, sphereCollider, capsuleCollider, meshCollider.
     */
    class Collider : public Component
    {
    private:
        bool isTrigger;
        Transform* transform;

    protected:
        bool isStatic;

    public:
        ColliderTypeE colliderType;

        Events::TEvent<Collider*> onCollisionEnter;
        Events::TEvent<Collider*> onCollisionExit;

        Collider();
        Collider(Transform* transform, bool isTrigger = false);
        virtual ~Collider() = default;

        virtual bool AcceptCollision(ColliderVisitor& visitor) = 0;
        virtual bool CheckCollision(const Collider& other) = 0;

        void SetTrigger(bool isTrigger);
        bool IsTrigger() const;

        void SetStatic(bool isStatic);
        bool IsStatic() const;

        void SetTransform(Transform* transform);
        Transform* GetTransform() const;

        void EmitCollisionEnter(Collider* other);
        void EmitCollisionExit(Collider* other);

        virtual Collider* GetInstance() = 0;

        Collider& operator=(const Collider& other);
    };

} // namespace Engine
