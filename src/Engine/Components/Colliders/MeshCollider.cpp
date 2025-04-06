#include "MeshCollider.h"

namespace Engine
{
    MeshCollider::MeshCollider(Transform* transform, bool isTrigger, float scale, Models::Mesh* mesh_p) :
        Collider(transform, isTrigger), _scale(scale), _mesh_p(mesh_p)
    {
    }

    bool MeshCollider::CheckCollision(const Collider& other)
    {
        if (isStatic || other.IsStatic())
            return false;

        // TODO: implement collision detection for MeshCollider

        return false;
    }

    MeshCollider& MeshCollider::operator=(const MeshCollider& other)
    {
        if (this == &other)
            return *this;

        Collider::operator=(other);
        _scale = other._scale;
        _mesh_p = other._mesh_p;

        return *this;
    }

    float MeshCollider::GetScale() const { return _scale; }
    void MeshCollider::SetScale(float scale) { _scale = scale; }

    Models::Mesh* MeshCollider::GetMesh() const { return _mesh_p; }
    void MeshCollider::SetMesh(Models::Mesh* mesh_p) { _mesh_p = mesh_p; }

} // namespace Engine
