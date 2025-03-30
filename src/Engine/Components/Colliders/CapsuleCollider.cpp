#include "CapsuleCollider.h"

CapsuleCollider::CapsuleCollider(Transform* transform, bool isTrigger) : Collider(transform, isTrigger) {}

bool CapsuleCollider::CheckCollision(const Collider& other)
{
    // Implementacja sprawdzania kolizji dla CapsuleCollider
    return false;
}
