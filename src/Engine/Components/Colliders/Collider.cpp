#include "Collider.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "MeshCollider.h"
#include "SphereCollider.h"

Collider::Collider() : isTrigger(false), transform(nullptr) {}

Collider::Collider(Transform* transform, bool isTrigger) : isTrigger(isTrigger), transform(transform) {}

void Collider::SetTrigger(bool isTrigger) { this->isTrigger = isTrigger; }

bool Collider::IsTrigger() const { return isTrigger; }

void Collider::SetTransform(Transform* transform) { this->transform = transform; }

Transform* Collider::GetTransform() const { return transform; }

void Collider::SetOnCollisionEnter(const std::function<void(Collider*)>& callback) { onCollisionEnter = callback; }

std::function<void(Collider*)> Collider::GetOnCollisionEnter() const { return onCollisionEnter; }

void Collider::SetOnCollisionExit(const std::function<void(Collider*)>& callback) { onCollisionExit = callback; }

std::function<void(Collider*)> Collider::GetOnCollisionExit() const { return onCollisionExit; }

MeshCollider* Collider::CreateMeshCollider(Transform* transform, bool isTrigger)
{
    return new MeshCollider(transform, isTrigger);
}

BoxCollider* Collider::CreateBoxCollider(Transform* transform, bool isTrigger)
{
    return new BoxCollider(transform, isTrigger);
}

SphereCollider* Collider::CreateSphereCollider(Transform* transform, bool isTrigger)
{
    return new SphereCollider(transform, isTrigger);
}

CapsuleCollider* Collider::CreateCapsuleCollider(Transform* transform, bool isTrigger)
{
    return new CapsuleCollider(transform, isTrigger);
}
