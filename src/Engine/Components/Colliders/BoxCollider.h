#ifndef BOX_COLLIDER_H
#define BOX_COLLIDER_H

#include "Collider.h"

class BoxCollider : public Collider
{
public:
    BoxCollider(Transform* transform, bool isTrigger = false, float width = 1.0f, float height = 1.0f,
                float depth = 1.0f);
    virtual bool CheckCollision(const Collider& other) override;

    float width;
    float height;
    float depth;
};

#endif // BOX_COLLIDER_H
