#include "SphereCollider.h"

namespace Engine
{
    SphereCollider::SphereCollider(Transform* transform, bool isTrigger, float radius) :
        Collider(transform, isTrigger), radius(radius)
    {
    }

    bool SphereCollider::AcceptCollision(ColliderVisitor& visitor)
    {
        visitor.VisitSphere(*this);
        return true;
    }

    bool SphereCollider::CheckCollision(const Collider& other)
    {
        if (isStatic || other.IsStatic())
            return false;

        // TODO: implement collision detection for SphereCollider

        return false;
    }

    SphereCollider& SphereCollider::operator=(const SphereCollider& other)
    {
        if (this == &other)
            return *this;

        Collider::operator=(other);
        radius = other.radius;

        return *this;
    }

    float SphereCollider::GetRadius() const { return radius; }
    void SphereCollider::SetRadius(float radius) { this->radius = radius; }

} // namespace Engine
