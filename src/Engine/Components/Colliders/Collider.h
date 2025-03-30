#ifndef COLLIDER_H
#define COLLIDER_H

#include <functional>
#include "Component.h"
#include "Transform.h"

using namespace Engine;

class Collider : public Component
{
public:
    Collider();
    Collider(Transform* transform, bool isTrigger = false);

    virtual ~Collider() = default;

    virtual bool CheckCollision(const Collider& other) = 0;

    void SetTrigger(bool isTrigger);
    bool IsTrigger() const;

    void SetTransform(Transform* transform);
    Transform* GetTransform() const;

    void SetOnCollisionEnter(const std::function<void(Collider*)>& callback);
    std::function<void(Collider*)> GetOnCollisionEnter() const;

    void SetOnCollisionExit(const std::function<void(Collider*)>& callback);
    std::function<void(Collider*)> GetOnCollisionExit() const;

    static MeshCollider* CreateMeshCollider(Transform* transform, bool isTrigger = false, float scale = 1.0f);
    static BoxCollider* CreateBoxCollider(Transform* transform, bool isTrigger = false, float width = 1.0f,
                                          float height = 1.0f, float depth = 1.0f);
    static SphereCollider* CreateSphereCollider(Transform* transform, bool isTrigger = false, float radius = 1.0f);
    static CapsuleCollider* CreateCapsuleCollider(Transform* transform, bool isTrigger = false, float radius = 1.0f,
                                                  float height = 2.0f);

private:
    bool isTrigger;
    Transform* transform;

    std::function<void(Collider*)> onCollisionEnter;
    std::function<void(Collider*)> onCollisionExit;
};

inline Collider::Collider() : isTrigger(false), transform(nullptr) {}

inline Collider::Collider(Transform* transform, bool isTrigger) : isTrigger(isTrigger), transform(transform) {}

inline void Collider::SetTrigger(bool isTrigger) { this->isTrigger = isTrigger; }

inline bool Collider::IsTrigger() const { return isTrigger; }

inline void Collider::SetTransform(Transform* transform) { this->transform = transform; }

inline Transform* Collider::GetTransform() const { return transform; }

inline void Collider::SetOnCollisionEnter(const std::function<void(Collider*)>& callback)
{
    onCollisionEnter = callback;
}

inline std::function<void(Collider*)> Collider::GetOnCollisionEnter() const { return onCollisionEnter; }

inline void Collider::SetOnCollisionExit(const std::function<void(Collider*)>& callback) { onCollisionExit = callback; }

inline std::function<void(Collider*)> Collider::GetOnCollisionExit() const { return onCollisionExit; }

inline MeshCollider* Collider::CreateMeshCollider(Transform* transform, bool isTrigger, float scale)
{
    return new MeshCollider(transform, isTrigger, scale);
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
