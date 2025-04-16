#pragma once

#include "../Component.h" // TODO: Fix later. I'm using this way because of indexing problem.
#include "../Transform.h" // Add this include to ensure the Transform class is fully defined
#include "Events/TAction.h"
#include "Events/TEvent.h"
#include "ColliderVisitor.h"
#include "Serialization/SerializationUtility.h"

namespace Engine
{
    class SpatialPartitioning;
    class Transform;

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

    protected:
        bool isStatic;
        bool isTrigger;

        Transform* transform;

        ConcreteColliderVisitor colliderVisitor;
        SpatialPartitioning* spatial;

    public:
        ColliderTypeE colliderType;

        Events::TEvent<Collider*> onCollisionEnter;
        Events::TEvent<Collider*> onCollisionExit;

        Collider();
        Collider(Transform* transform, bool isTrigger = false, bool isStatic = false, SpatialPartitioning* spatial = nullptr);
        virtual ~Collider() = default;

        virtual bool AcceptCollision(ColliderVisitor& visitor) = 0;
        
        //virtual bool CheckCollision(const Collider& other) = 0;

        void Start() override;
        void OnDestroy() override;

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

        void Update();
        
    };

} // namespace Engine
