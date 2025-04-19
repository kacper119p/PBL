#include "SpatialPartitioning.h"

namespace Engine
{
    SpatialPartitioning::SpatialPartitioning(float cellSize) : cellSize(cellSize) {}

    glm::ivec3 SpatialPartitioning::GetCellIndex(const glm::vec3& position) const
    {
        return glm::ivec3(static_cast<int>(std::floor(position.x / cellSize)),
                          static_cast<int>(std::floor(position.y / cellSize)),
                          static_cast<int>(std::floor(position.z / cellSize)));
    }

    void SpatialPartitioning::AddCollider(Collider* collider)
    {
        glm::vec3 position = collider->GetTransform()->GetPositionWorldSpace();

        glm::ivec3 cellIndex = GetCellIndex(position);

        grid[cellIndex].push_back(collider);
    }

    void SpatialPartitioning::RemoveCollider(Collider* collider)
    {
        glm::vec3 position = collider->GetTransform()->GetPositionWorldSpace();

        glm::ivec3 cellIndex = GetCellIndex(position);

        auto& cell = grid[cellIndex];
        cell.erase(std::remove(cell.begin(), cell.end(), collider), cell.end());

        if (cell.empty())
        {
            grid.erase(cellIndex);
        }
    }

    std::vector<Collider*> SpatialPartitioning::GetPotentialCollisions(Collider* collider)
    {
        glm::vec3 position = collider->GetTransform()->GetPositionWorldSpace();

        glm::ivec3 cellIndex = GetCellIndex(position);

        std::vector<Collider*> potentialCollisions;
        for (int x = -1; x <= 1; ++x)
        {
            for (int y = -1; y <= 1; ++y)
            {
                for (int z = -1; z <= 1; ++z)
                {
                    glm::ivec3 neighborIndex = cellIndex + glm::ivec3(x, y, z);
                    if (grid.find(neighborIndex) != grid.end())
                    {
                        const auto& cell = grid[neighborIndex];
                        potentialCollisions.insert(potentialCollisions.end(), cell.begin(), cell.end());
                    }
                }
            }
        }

        return potentialCollisions;
    }

    void SpatialPartitioning::Update(float deltaTime)
    {
    }
} // namespace Engine
