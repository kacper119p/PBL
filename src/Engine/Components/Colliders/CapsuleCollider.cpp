#include "CapsuleCollider.h"

CapsuleCollider::CapsuleCollider(Transform* transform, bool isTrigger, float radius, float height) :
    Collider(transform, isTrigger), _radius(radius), _height(height)
{
}

bool CapsuleCollider::CheckCollision(const Collider& other)
{
    // TODO: implement collision detection for CapsuleCollider
    return false;
}
