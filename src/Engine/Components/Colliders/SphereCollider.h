#ifndef SPHERE_COLLIDER_H
#define SPHERE_COLLIDER_H

#include "Collider.h"

class SphereCollider : public Collider
{
public:
    SphereCollider(Transform* transform, bool isTrigger = false, float radius = 1.0f);
    virtual bool CheckCollision(const Collider& other) override;

    float radius;
};

#endif // SPHERE_COLLIDER_H
