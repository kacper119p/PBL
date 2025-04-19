#pragma once

#include "Collider.h"

namespace Engine
{
    /**
     * @brief Box collider class.
     * @details Represents a box-shaped collider.
    */

    class CocreteColliderVisitor;

    class BoxCollider : public Collider
    {
    private:
        float _width;
        float _height;
        float _depth;

    public:
        BoxCollider() = default;
        BoxCollider(Transform* transform, bool isTrigger = false, float width = 1.0f, float height = 1.0f,
                    float depth = 1.0f);

        virtual bool AcceptCollision(ColliderVisitor& visitor) override;
        //virtual bool CheckCollision(const Collider& other) override;

        inline virtual Collider* GetInstance() override { return this; }

        void Start() override;
        void OnDestroy() override;

        float GetWidth() const;
        void SetWidth(float width);

        float GetHeight() const;
        void SetHeight(float height);

        float GetDepth() const;
        void SetDepth(float depth);

        void DrawDebugMesh() override;

        BoxCollider& operator=(const BoxCollider& other);

        SERIALIZATION_EXPORT_CLASS(BoxCollider)
        
    };

} // namespace Engine
