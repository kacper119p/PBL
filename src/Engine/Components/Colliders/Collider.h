#ifndef COLLIDER_H
#define COLLIDER_H

#include <functional>
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "Component.h"
#include "MeshCollider.h"
#include "SphereCollider.h"
#include "Transform.h"
#include "src/events/Event.h" // Zak³adam, ¿e Event.h zawiera definicjê systemu eventów

using namespace Engine;

class Collider : public Component
{
private:
    bool isTrigger;
    Transform* transform;

    Event<Collider*> onCollisionEnter;
    Event<Collider*> onCollisionExit;

public:
    Collider();
    Collider(Transform* transform, bool isTrigger = false);

    virtual ~Collider() = default;

    virtual bool CheckCollision(const Collider& other) = 0;

    void SetTrigger(bool isTrigger);
    bool IsTrigger() const;

    void SetTransform(Transform* transform);
    Transform* GetTransform() const;

    void AddOnCollisionEnterListener(const std::function<void(Collider*)>& listener);
    void RemoveOnCollisionEnterListener(const std::function<void(Collider*)>& listener);

    void AddOnCollisionExitListener(const std::function<void(Collider*)>& listener);
    void RemoveOnCollisionExitListener(const std::function<void(Collider*)>& listener);

    static MeshCollider* CreateMeshCollider(Transform* transform, bool isTrigger = false, float xScale = 1.0f,
                                            float yScale = 1.0f, float zScale = 1.0f, Engine::Mesh* mesh_p = nullptr);
    static BoxCollider* CreateBoxCollider(Transform* transform, bool isTrigger = false, float width = 1.0f,
                                          float height = 1.0f, float depth = 1.0f);
    static SphereCollider* CreateSphereCollider(Transform* transform, bool isTrigger = false, float radius = 1.0f);
    static CapsuleCollider* CreateCapsuleCollider(Transform* transform, bool isTrigger = false, float radius = 1.0f,
                                                  float height = 2.0f);
};

inline Collider::Collider() : isTrigger(false), transform(nullptr) {}

inline Collider::Collider(Transform* transform, bool isTrigger) : isTrigger(isTrigger), transform(transform) {}

inline void Collider::SetTrigger(bool isTrigger) { this->isTrigger = isTrigger; }

inline bool Collider::IsTrigger() const { return isTrigger; }

inline void Collider::SetTransform(Transform* transform) { this->transform = transform; }

inline Transform* Collider::GetTransform() const { return transform; }

inline void Collider::AddOnCollisionEnterListener(const std::function<void(Collider*)>& listener)
{
    onCollisionEnter.AddListener(listener);
}

inline void Collider::RemoveOnCollisionEnterListener(const std::function<void(Collider*)>& listener)
{
    onCollisionEnter.RemoveListener(listener);
}

inline void Collider::AddOnCollisionExitListener(const std::function<void(Collider*)>& listener)
{
    onCollisionExit.AddListener(listener);
}

inline void Collider::RemoveOnCollisionExitListener(const std::function<void(Collider*)>& listener)
{
    onCollisionExit.RemoveListener(listener);
}

inline MeshCollider* Collider::CreateMeshCollider(Transform* transform, bool isTrigger, float xScale, float yScale,
                                                  float zScale, Engine::Mesh* mesh_p)
{
    return new MeshCollider(transform, isTrigger, xScale, yScale, zScale, mesh_p);
}

inline BoxCollider* Collider::CreateBoxCollider(Transform* transform, bool isTrigger, float width, float height,
                                                float depth)
{
    return new BoxCollider(transform, isTrigger, width, height, depth);
}

inline SphereCollider* Collider::CreateSphereCollider(Transform* transform, bool isTrigger, float radius)
{
    return new SphereCollider(transform, isTrigger, radius);
}

inline CapsuleCollider* Collider::CreateCapsuleCollider(Transform* transform, bool isTrigger, float radius,
                                                        float height)
{
    return new CapsuleCollider(transform, isTrigger, radius, height);
}

#endif // COLLIDER_H
