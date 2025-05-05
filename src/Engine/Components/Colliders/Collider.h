#pragma once

#include "../Component.h" // TODO: Fix later. I'm using this way because of indexing problem.
#include "../Transform.h" // TODO: Fix later. I'm using this way because of indexing problem.
#include "Events/TAction.h"
#include "Events/TEvent.h"
#include "ColliderVisitor.h"
#include "Serialization/SerializationUtility.h"
#include "../../EngineObjects/Entity.h" // TODO: Fix later. I'm using this way because of indexing problem.
#include "../Interfaces/IUpdateable.h"
#include "../../EngineObjects/UpdateManager.h"
#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <cmath>
#include "Engine/Components/Renderers/Renderer.h"

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
    class Collider : public Renderer
    {

    protected:
        bool isStatic;
        bool isTrigger;
        glm::ivec3 currentCellIndex = glm::ivec3(0, 0, 0); // non-definable by user
        SpatialPartitioning* spatial;

    public:
        ColliderTypeE colliderType;
        Events::TEvent<Collider*> onCollisionEnter;
        Events::TEvent<Collider*> onCollisionExit;

        /// TODO: move those to protected and add getters + setters
        Transform* transform;
        ConcreteColliderVisitor colliderVisitor;
        

        // TODO: remove when spatial fully implemented
        std::vector<Collider*> colliders;
        bool isColliding;

        Collider();
        Collider(Transform* transform, bool isTrigger = false, bool isStatic = false);
        virtual ~Collider();

        virtual bool AcceptCollision(ColliderVisitor& visitor) = 0;

        void SetTrigger(bool isTrigger);
        bool IsTrigger() const;

        void SetStatic(bool isStatic);
        bool IsStatic() const;

        void SetTransform(Transform* transform);
        Transform* GetTransform() const;

        void EmitCollisionEnter(Collider* other);
        void EmitCollisionExit(Collider* other);

        virtual Collider* GetInstance() = 0;
        virtual void DrawDebugMesh(const CameraRenderData& RenderData) = 0;

        bool GetCollisionStatus();

        Collider& operator=(const Collider& other);
        bool operator==(const Collider& other) const;
        bool operator==(const Collider* other) const
        {
            return this == other;
        }

        void RenderDepth(const CameraRenderData& RenderData) override;

        void Render(const CameraRenderData& RenderData) override;

        void RenderDirectionalShadows(const CameraRenderData& RenderData) override;

        void RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                    const glm::mat4* SpaceTransformMatrices) override;

        // TODO: remove when spatial fully implemented
        void AddColliderToSpatial(Collider* collider);
        

    };

} // namespace Engine
