#include "Collider.h"
#include "SpatialPartitioning.h"
#include "../Transform.h" // TODO: Fix later. I'm using this way because of indexing problems
#include "Serialization/SerializationUtility.h"
#include "spdlog/spdlog.h"

namespace Engine
{
    
    Collider::Collider() : isTrigger(false), transform(nullptr), spatial(&SpatialPartitioning::GetInstance())
    {
        this->colliderVisitor = ConcreteColliderVisitor();
        isColliding = false; // TODO: to be removed, just for debug purposes
    }

    Collider::Collider(Transform* transform, bool isTrigger, bool isStatic) 
        :
        isTrigger(isTrigger), isStatic(isStatic), transform(transform), spatial(&SpatialPartitioning::GetInstance())
    {
        this->colliderVisitor = ConcreteColliderVisitor(this);
        colliderType = BOX; // Default type, can be changed in derived classes
        isColliding = false; // TODO: to be removed, just for debug purposes
    }

    Collider::~Collider() 
    { 
    }

    void Collider::SetTrigger(bool isTrigger) { this->isTrigger = isTrigger; }

    bool Collider::IsTrigger() const { return isTrigger; }

    void Collider::SetStatic(bool isStatic) { this->isStatic = isStatic; }

    bool Collider::IsStatic() const { return isStatic; }

    void Collider::SetTransform(Transform* transform) { this->transform = transform; }

    Transform* Collider::GetTransform() const { return transform; }

    void Collider::EmitCollisionEnter(Collider* other) { onCollisionEnter.Invoke(other); }

    void Collider::EmitCollisionExit(Collider* other) { onCollisionExit.Invoke(other); }

    bool Collider::GetCollisionStatus() { return isColliding;}

    Collider& Collider::operator=(const Collider& other)
    {
        if (this == &other)
            return *this;

        isTrigger = other.isTrigger;
        transform = other.transform;
        onCollisionEnter = other.onCollisionEnter;
        onCollisionExit = other.onCollisionExit;

        return *this;
    }

    bool Collider::operator==(const Collider& other) const { return this->GetID() == other.GetID();
    }

    void Collider::RenderDepth(const CameraRenderData& RenderData) {}

    void Collider::Render(const CameraRenderData& RenderData) {}

    void Collider::RenderDirectionalShadows(const CameraRenderData& RenderData) {}

    void Collider::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                const glm::mat4* SpaceTransformMatrices) {}

    void Collider::AddColliderToSpatial(Collider* collider)
    { 
        colliders.push_back(collider);
    }

} // namespace Engine
