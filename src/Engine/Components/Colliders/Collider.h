#pragma once

#include "../Transform.h" // TODO: Fix later. I'm using this way because of indexing problem.
#include "Events/TEvent.h"
#include "ColliderVisitor.h"
#include "Serialization/SerializationUtility.h"
#include <glm/glm.hpp>
#include "Engine/Components/Renderers/Renderer.h"
#include "Events/Action.h"
#include "PrimitiveMeshes.h"
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
    class Collider
#if EDITOR
            : public Renderer
#else
            : public Component
#endif
    {

    protected:
        bool isStatic;
        bool isTrigger;
        glm::ivec3 CurrentCellIndex = glm::ivec3(0, 0, 0); // non-definable by user
        SpatialPartitioning* Spatial;

        // TODO: remove when rigidbody fully implemented
        const glm::vec3 Gravity = glm::vec3(0.0f, -9.81f, 0.0f);
        // TODO END

    public:
        ColliderTypeE colliderType;
        Events::TEvent<Collider*> OnCollision;
        Events::TEvent<Collider*> OnTrigger;
        /// TODO: move those to protected and add getters + setters
        Transform* transform;
        ColliderVisitor colliderVisitor;
        PrimitiveMesh mesh;
        bool isColliding;

        Collider();

#if EDITOR
        ~Collider() override;
#else
        virtual ~Collider();
#endif

        virtual glm::vec3 GetBoundingBox() const = 0;

        virtual bool AcceptCollision(ColliderVisitor& visitor) = 0;

        virtual glm::mat3 CalculateInertiaTensorBody(float mass) const = 0;

        std::vector<Collider*> SphereOverlap(glm::vec3& position, float Radius) const 
        {
            return Spatial->QuerySphere(position, Radius);
        }

        void SetTrigger(bool isTrigger)
        {
            this->isTrigger = isTrigger;
        }

        bool IsTrigger() const
        {
            return isTrigger;
        }

        void SetStatic(bool isStatic)
        {
            this->isStatic = isStatic;
        }

        bool IsStatic() const
        {
            return isStatic;
        }

        PrimitiveMesh* GetMesh() { return &mesh; }

        void SetTransform(Transform* transform)
        {
            this->transform = transform;
        }

        [[nodiscard]] Transform* GetTransform() const
        {
            return transform;
        }

        void EmitCollision(Collider* Other)
        {
            OnCollision.Invoke(Other);
        }

        void EmitTrigger(Collider* Other)
        {
            OnTrigger.Invoke(Other);
        }

        void OnTriggerAddListener(const Events::TAction<Collider*>& Listener)
        {
            OnTrigger.AddListener(Listener);
        }

        void OnTriggerRemoveListener(const Events::TAction<Collider*>& Listener)
        {
            OnTrigger.RemoveListener(Listener);
        }

        void OnCollisionAddListener(const Events::TAction<Collider*>& Listener)
        {
            OnCollision.AddListener(Listener);
        }

        void OnCollisionRemoveListener(const Events::TAction<Collider*>& Listener)
        {
            OnCollision.RemoveListener(Listener);
        }

        virtual Collider* GetInstance() = 0;

        [[nodiscard]] bool GetCollisionStatus() const
        {
            return isColliding;
        }

        Collider& operator=(const Collider& Other);

        bool operator==(const Collider& other) const;

        bool operator==(const Collider* other) const
        {
            return this == other;
        }

       

        void Start() override;

        void OnDestroy() override;

        void Update(float DeltaTime);

        float GetRandomFloat(float Min, float Max);

#if EDITOR
        void DrawImGui() override
        {
        };
#endif
    };

} // namespace Engine
