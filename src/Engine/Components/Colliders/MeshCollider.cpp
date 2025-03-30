#include "MeshCollider.h"
#include <algorithm>

MeshCollider::MeshCollider(Transform* transform, bool isTrigger, float xScale, float yScale, float zScale,
                           Engine::Mesh* mesh_p) :
    Collider(transform, isTrigger), xScale(xScale), yScale(yScale), zScale(zScale), mesh_p(mesh_p)
{
}

bool MeshCollider::CheckCollision(const Collider& other)
{
    const MeshCollider* otherMeshCollider = dynamic_cast<const MeshCollider*>(&other);
    if (otherMeshCollider)
    {
        return CheckMeshCollision(*otherMeshCollider);
    }
    return false;
}

bool MeshCollider::CheckMeshCollision(const MeshCollider& other) const
{
    auto [minA, maxA] = GetAABB();
    auto [minB, maxB] = other.GetAABB();
    return (minA.x <= maxB.x && maxA.x >= minB.x) && (minA.y <= maxB.y && maxA.y >= minB.y) &&
           (minA.z <= maxB.z && maxA.z >= minB.z);
}

std::pair<glm::vec3, glm::vec3> MeshCollider::GetAABB() const
{
    glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::lowest());
    for (const auto& vertex : mesh_p->vertices)
    {
        glm::vec3 transformedVertex = transform->LocalToWorldMatrix * glm::vec4(vertex.position, 1.0f);
        minAABB = glm::min(minAABB, transformedVertex);
        maxAABB = glm::max(maxAABB, transformedVertex);
    }
    return {minAABB, maxAABB};
}
