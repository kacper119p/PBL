#include "Collider.h"
#include "SpatialPartitioning.h"
#include "../Transform.h" // TODO: Fix later. I'm using this way because of indexing problems
#include "Serialization/SerializationUtility.h"

namespace Engine
{
    
    Collider::Collider() : isTrigger(false), transform(nullptr) {
        this->colliderVisitor = ConcreteColliderVisitor();
    }

    Collider::Collider(Transform* transform, bool isTrigger, bool isStatic, SpatialPartitioning* spatialParam) 
        : isTrigger(isTrigger), isStatic(isStatic), transform(transform), spatial(spatialParam) {
        this->colliderVisitor = ConcreteColliderVisitor(spatialParam, this);
        colliderType = BOX; // Default type, can be changed in derived classes
    }

    void Collider::SetTrigger(bool isTrigger) { this->isTrigger = isTrigger; }

    bool Collider::IsTrigger() const { return isTrigger; }

    void Collider::SetStatic(bool isStatic) { this->isStatic = isStatic; }

    bool Collider::IsStatic() const { return isStatic; }

    void Collider::SetTransform(Transform* transform) { this->transform = transform; }

    Transform* Collider::GetTransform() const { return transform; }

    void Collider::EmitCollisionEnter(Collider* other) { onCollisionEnter.Invoke(other); }

    void Collider::EmitCollisionExit(Collider* other) { onCollisionExit.Invoke(other); }

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

    void Collider::Update(float deltaTime) {  
       DrawDebugMesh();
       colliderVisitor.ManageCollisions();  
    }

} // namespace Engine
