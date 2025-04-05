#include "SphereCollider.h"

SphereCollider::SphereCollider(Transform* transform, bool isTrigger, float radius) :
    Collider(transform, isTrigger), radius(radius)
{
}

bool SphereCollider::CheckCollision(const Collider& other)
{
    // TODO: implement collision detection for SphereCollider
    return false;
}
