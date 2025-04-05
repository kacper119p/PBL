#ifndef BOX_COLLIDER_H
#define BOX_COLLIDER_H

#include "Collider.h"

class BoxCollider : public Collider
{
public:
    float _width;
    float _height;

    BoxCollider(Transform* transform, bool isTrigger = false, float width = 1.0f, float height = 1.0f,
                float depth = 1.0f);

    virtual bool CheckCollision(const Collider& other) override;
};

#endif // BOX_COLLIDER_H
