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

#endif // COLLIDER_H
