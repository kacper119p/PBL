#ifndef CAPSULE_COLLIDER_H
#define CAPSULE_COLLIDER_H

#include "Collider.h"

class CapsuleCollider : public Collider
{
public:
    CapsuleCollider(Transform* transform, bool isTrigger = false, float radius = 1.0f, float height = 2.0f);
    virtual bool CheckCollision(const Collider& other) override;

    float radius;
    float height;
};

#endif // CAPSULE_COLLIDER_H
