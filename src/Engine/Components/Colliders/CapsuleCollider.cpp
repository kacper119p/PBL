#include "CapsuleCollider.h"

namespace Engine
{
    CapsuleCollider::CapsuleCollider(Transform* transform, bool isTrigger, float radius, float height) :
        Collider(transform, isTrigger), _radius(radius), _height(height)
    {
        this->colliderType = CAPSULE;
    }

    bool CapsuleCollider::AcceptCollision(ColliderVisitor& visitor)
    {
        visitor.ResolveCollisionCapsule(*this);
        return true;
    }

    CapsuleCollider& CapsuleCollider::operator=(const CapsuleCollider& other)
    {
        if (this == &other)
            return *this;

        Collider::operator=(other);
        _radius = other._radius;
        _height = other._height;

        return *this;
    }

    float CapsuleCollider::GetRadius() const { return _radius; }
    void CapsuleCollider::SetRadius(float radius) { _radius = radius; }

    float CapsuleCollider::GetHeight() const { return _height; }
    void CapsuleCollider::SetHeight(float height) { _height = height; }

} // namespace Engine
