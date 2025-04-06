#include "BoxCollider.h"

namespace Engine
{
    BoxCollider::BoxCollider(Transform* transform, bool isTrigger, float width, float height, float depth) :
        Collider(transform, isTrigger), _width(width), _height(height), _depth(depth)
    {
    }

    bool BoxCollider::CheckCollision(const Collider& other)
    {
        // TODO: implement collision detection for BoxCollider
        return false;
    }

    BoxCollider& BoxCollider::operator=(const BoxCollider& other)
    {
        if (this == &other)
            return *this;

        Collider::operator=(other);
        _width = other._width;
        _height = other._height;
        _depth = other._depth;

        return *this;
    }

    float BoxCollider::GetWidth() const { return _width; }
    void BoxCollider::SetWidth(float width) { _width = width; }

    float BoxCollider::GetHeight() const { return _height; }
    void BoxCollider::SetHeight(float height) { _height = height; }

    float BoxCollider::GetDepth() const { return _depth; }
    void BoxCollider::SetDepth(float depth) { _depth = depth; }
} // namespace Engine
