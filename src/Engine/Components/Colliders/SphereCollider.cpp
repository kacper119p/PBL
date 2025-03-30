#include "SphereCollider.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "MeshCollider.h"

BoxCollider::BoxCollider(Transform* transform, bool isTrigger, float width, float height, float depth) :
    Collider(transform, isTrigger), width(width), height(height), depth(depth)
{
}

bool BoxCollider::CheckCollision(const Collider& other)
{
    // Implementacja sprawdzania kolizji dla BoxCollider
    return false;
}

SphereCollider::SphereCollider(Transform* transform, bool isTrigger, float radius) :
    Collider(transform, isTrigger), radius(radius)
{
}

bool SphereCollider::CheckCollision(const Collider& other)
{
    // Implementacja sprawdzania kolizji dla SphereCollider
    return false;
}

CapsuleCollider::CapsuleCollider(Transform* transform, bool isTrigger, float radius, float height) :
    Collider(transform, isTrigger), radius(radius), height(height)
{
}

bool CapsuleCollider::CheckCollision(const Collider& other)
{
    // Implementacja sprawdzania kolizji dla CapsuleCollider
    return false;
}

MeshCollider::MeshCollider(Transform* transform, bool isTrigger, float scale) :
    Collider(transform, isTrigger), scale(scale)
{
}

bool MeshCollider::CheckCollision(const Collider& other)
{
    // Implementacja sprawdzania kolizji dla MeshCollider
    return false;
}
