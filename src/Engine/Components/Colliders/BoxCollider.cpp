#include "BoxCollider.h"

BoxCollider::BoxCollider(Transform* transform, bool isTrigger) : Collider(transform, isTrigger) {}

bool BoxCollider::CheckCollision(const Collider& other)
{
    // Implementacja sprawdzania kolizji dla BoxCollider
    return false;
}
