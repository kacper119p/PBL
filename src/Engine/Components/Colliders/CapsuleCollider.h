#ifndef CAPSULE_COLLIDER_H
#define CAPSULE_COLLIDER_H

#include "Collider.h"

class CapsuleCollider : public Collider
{
private:
    float _radius;
    float _height;

public:
    CapsuleCollider(Transform* transform, bool isTrigger = false, float radius = 1.0f, float height = 2.0f);
    virtual bool CheckCollision(const Collider& other) override;
    
};

#endif // CAPSULE_COLLIDER_H
