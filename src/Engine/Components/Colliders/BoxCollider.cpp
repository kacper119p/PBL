#include "BoxCollider.h"

BoxCollider::BoxCollider(Transform* transform, bool isTrigger, float width, float height, float depth) :
    Collider(transform, isTrigger), _width(width), _height(height)
{
}

bool BoxCollider::CheckCollision(const Collider& other)
{
    // TODO: implement collision detection for BoxCollider
    return false;
}
