#include "Collider.h"

namespace Engine
{
    Collider::Collider() : isTrigger(false), transform(nullptr) {}

    Collider::Collider(Transform* transform, bool isTrigger) : isTrigger(isTrigger), transform(transform) {}

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

} // namespace Engine
