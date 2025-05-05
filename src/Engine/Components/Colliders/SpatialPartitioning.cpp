#include "SpatialPartitioning.h"
#include <algorithm>
#include <cmath>
#include <spdlog/spdlog.h>

namespace Engine
{
    SpatialPartitioning* SpatialPartitioning::instance_ = nullptr;

    SpatialPartitioning::SpatialPartitioning() :
        cellSize(100.0f), gridDimensions(glm::ivec3(10, 10, 10)), origin(glm::vec3(-10.0f,-10.0f,-10.0f))
    {
        grid.resize(gridDimensions.x);
        for (int x = 0; x < gridDimensions.x; ++x)
        {
            grid[x].resize(gridDimensions.y);
            for (int y = 0; y < gridDimensions.y; ++y)
            {
                grid[x][y].resize(gridDimensions.z);
            }
        }
    }

    SpatialPartitioning::~SpatialPartitioning() { }

    SpatialPartitioning& SpatialPartitioning::GetInstance()
    {
        if (!instance_)
        {
            instance_ = new SpatialPartitioning();
        }
        return *instance_;
    }

    void SpatialPartitioning::DestroyInstance()
    {
        if (!instance_)
        {
            delete &instance_;
            instance_ = nullptr;
        }
    }

   glm::ivec3 SpatialPartitioning::GetCellIndex(const glm::vec3& position) const
    {
        glm::vec3 localPos = position - origin;

        return glm::ivec3(static_cast<int>(std::floor(localPos.x / cellSize)),
                          static_cast<int>(std::floor(localPos.y / cellSize)),
                          static_cast<int>(std::floor(localPos.z / cellSize)));
    }

    bool SpatialPartitioning::IsValidIndex(const glm::ivec3& index) const
    {
        bool valid = index.x >= 0 && index.x < gridDimensions.x && index.y >= 0 && index.y < gridDimensions.y &&
                     index.z >= 0 && index.z < gridDimensions.z;
        return valid;
    }

    void SpatialPartitioning::AddCollider(Collider* collider)
    {
        glm::vec3 position = collider->GetTransform()->GetPositionWorldSpace();
        glm::ivec3 index = GetCellIndex(position);

        if (IsValidIndex(index))
        {
            grid[index.x][index.y][index.z].push_back(collider);
        }
    }

    void SpatialPartitioning::RemoveCollider(Collider* collider)
    {
        glm::vec3 position = collider->GetTransform()->GetPositionWorldSpace();
        glm::ivec3 index = GetCellIndex(position);

        if (IsValidIndex(index))
        {
            auto& cell = grid[index.x][index.y][index.z];
            cell.erase(std::remove(cell.begin(), cell.end(), collider), cell.end());
        }
    }

    std::vector<Collider*> SpatialPartitioning::GetPotentialCollisions(Collider* collider)
    {
        glm::vec3 position = collider->GetTransform()->GetPositionWorldSpace();
        glm::ivec3 centerIndex = GetCellIndex(position);

        std::vector<Collider*> potentialCollisions;

        for (int x = -1; x <= 1; ++x)
        {
            for (int y = -1; y <= 1; ++y)
            {
                for (int z = -1; z <= 1; ++z)
                {
                    glm::ivec3 neighborIndex = centerIndex + glm::ivec3(x, y, z);
                    if (IsValidIndex(neighborIndex))
                    {
                        const auto& cell = grid[neighborIndex.x][neighborIndex.y][neighborIndex.z];
                        potentialCollisions.insert(potentialCollisions.end(), cell.begin(), cell.end());
                    }
                }
            }
        }

        return potentialCollisions;
    }

    void SpatialPartitioning::SetCellSize(float newCellSize)
    {
        if (newCellSize <= 0.0f)
        {
            spdlog::error("Cell size must be greater than 0.");
            return;
        }

        cellSize = newCellSize;

        glm::vec3 maxPosition = origin + glm::vec3(gridDimensions) * cellSize;
        gridDimensions = glm::ivec3(static_cast<int>(std::ceil((maxPosition.x - origin.x) / cellSize)),
                                    static_cast<int>(std::ceil((maxPosition.y - origin.y) / cellSize)),
                                    static_cast<int>(std::ceil((maxPosition.z - origin.z) / cellSize)));

        std::vector<std::vector<std::vector<std::vector<Collider*>>>> newGrid;
        newGrid.resize(gridDimensions.x);
        for (int x = 0; x < gridDimensions.x; ++x)
        {
            newGrid[x].resize(gridDimensions.y);
            for (int y = 0; y < gridDimensions.y; ++y)
            {
                newGrid[x][y].resize(gridDimensions.z);
            }
        }

        for (int x = 0; x < grid.size(); ++x)
        {
            for (int y = 0; y < grid[x].size(); ++y)
            {
                for (int z = 0; z < grid[x][y].size(); ++z)
                {
                    for (Collider* collider : grid[x][y][z])
                    {
                        glm::vec3 position = collider->GetTransform()->GetPositionWorldSpace();
                        glm::ivec3 newIndex = GetCellIndex(position);

                        if (IsValidIndex(newIndex))
                        {
                            newGrid[newIndex.x][newIndex.y][newIndex.z].push_back(collider);
                        }
                    }
                }
            }
        }
        grid = std::move(newGrid);

        spdlog::info("Grid resized. New cell size: {}, New dimensions: ({}, {}, {})", cellSize, gridDimensions.x,
                     gridDimensions.y, gridDimensions.z);
    }
} // namespace Engine
