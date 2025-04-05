#include "MeshCollider.h"
using namespace Engine;

MeshCollider::MeshCollider(Transform* transform, bool isTrigger, float scale,
                           Mesh* mesh_p) :
    Collider(transform, isTrigger), _scale(scale), _mesh_p(mesh_p)
{
}

bool MeshCollider::CheckCollision(const Collider& other)
{
    // TODO: implement collision detection for MeshCollider
    return false;
}
