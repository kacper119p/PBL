#pragma once

#include "Collider.h"
#include "Models/Mesh.h" // TODO: Fix later. Double inclusion. I'm using this way because of indexing problem.

namespace Engine
{
    /**
     * @brief Mesh collider class.
     * @details Represents a mesh-shaped collider.
     */
    class MeshCollider : public Collider
    {
    public:
        float _scale;
        Models::Mesh* _mesh_p;

        MeshCollider() = default;

        MeshCollider(Transform* transform, bool isTrigger = false, float scale = 1.0f, Models::Mesh* mesh_p = nullptr);

        virtual bool CheckCollision(const Collider& other) override;

        inline virtual Collider* GetInstance() override { return this; }

        // Getters and setters
        float GetScale() const;
        void SetScale(float scale);

        Models::Mesh* GetMesh() const;
        void SetMesh(Models::Mesh* mesh_p);

        // Assignment operator
        MeshCollider& operator=(const MeshCollider& other);
    };

} // namespace Engine
