#pragma once

#include "../Component.h" // TODO: Fix later. I'm using this way because of indexing problem.
#include "../Transform.h" // TODO: Fix later. I'm using this way because of indexing problem.

#include "Events/TAction.h"
#include "Events/TEvent.h"

namespace Engine
{
    enum ColliderType
    {
        Box,
        Sphere,
        Capsule,
        Mesh
    };

    /*
     * @brief Base class for all colliders. Subtypes: boxCollider, sphereCollider, capsuleCollider, meshCollider.
     */
    class Collider : public Component
    {
    private:
        bool isTrigger;
        Transform* transform;

    public:
        Events::TEvent<Collider*> onCollisionEnter;
        Events::TEvent<Collider*> onCollisionExit;

        Collider();
        Collider(Transform* transform, bool isTrigger = false);
        virtual ~Collider() = default;

        virtual bool CheckCollision(const Collider& other) = 0;

        void SetTrigger(bool isTrigger);
        bool IsTrigger() const;

        void SetTransform(Transform* transform);
        Transform* GetTransform() const;

        void EmitCollisionEnter(Collider* other);
        void EmitCollisionExit(Collider* other);

        virtual Collider* GetInstance() = 0;

        Collider& operator=(const Collider& other);
    };

} // namespace Engine
