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

void Collider::AddOnCollisionEnterListener(const std::function<void(Collider*)>& listener)
{
    onCollisionEnter.AddListener(listener);
}

void Collider::RemoveOnCollisionEnterListener(const std::function<void(Collider*)>& listener)
{
    onCollisionEnter.RemoveListener(listener);
}

void Collider::AddOnCollisionExitListener(const std::function<void(Collider*)>& listener)
{
    onCollisionExit.AddListener(listener);
}

void Collider::RemoveOnCollisionExitListener(const std::function<void(Collider*)>& listener)
{
    onCollisionExit.RemoveListener(listener);
}

MeshCollider* Collider::CreateMeshCollider(Transform* transform, bool isTrigger, float xScale, float yScale,
                                           float zScale, Engine::Mesh* mesh_p)
{
    return new MeshCollider(transform, isTrigger, xScale, yScale, zScale, mesh_p);
}

BoxCollider* Collider::CreateBoxCollider(Transform* transform, bool isTrigger, float width, float height, float depth)
{
    return new BoxCollider(transform, isTrigger, width, height, depth);
}

SphereCollider* Collider::CreateSphereCollider(Transform* transform, bool isTrigger, float radius)
{
    return new SphereCollider(transform, isTrigger, radius);
}

CapsuleCollider* Collider::CreateCapsuleCollider(Transform* transform, bool isTrigger, float radius, float height)
{
    return new CapsuleCollider(transform, isTrigger, radius, height);
}
