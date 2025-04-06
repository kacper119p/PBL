#pragma once

#include "Collider.h"

namespace Engine
{
    /**
     * @brief Sphere collider class.
     * @details Represents a sphere-shaped collider.
     */
    class SphereCollider : public Collider, ColliderVisitor
    {

    public:
        float radius;

        SphereCollider() = default;

        SphereCollider(Transform* transform, bool isTrigger = false, float radius = 1.0f);

        virtual bool AcceptCollision(ColliderVisitor& visitor) override;
        virtual bool CheckCollision(const Collider& other) override;

        inline virtual Collider* GetInstance() override { return this; }

        float GetRadius() const;
        void SetRadius(float radius);

        SphereCollider& operator=(const SphereCollider& other);
    };

} // namespace Engine
