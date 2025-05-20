#include "SpatialPartitioning.h"
#include <algorithm>
#include <cmath>
#include <spdlog/spdlog.h>
#include <iostream>

namespace Engine
{
    SpatialPartitioning* SpatialPartitioning::instance_ = nullptr;

    SpatialPartitioning::SpatialPartitioning() :
        cellSize(3.0f), gridDimensions(glm::ivec3(10, 10, 10)), origin(glm::vec3(-10.0f,-10.0f,-10.0f))
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

   std::vector<glm::ivec3> SpatialPartitioning::GetOccupiedCells(const glm::vec3& position,
                                                                  const glm::vec3& size) const
    {
        glm::vec3 minBounds = position - size * 0.5f;
        glm::vec3 maxBounds = position + size * 0.5f;

        glm::ivec3 minIndex = GetCellIndex(minBounds);
        glm::ivec3 maxIndex = GetCellIndex(maxBounds);

        std::vector<glm::ivec3> occupiedCells;
        for (int x = minIndex.x; x <= maxIndex.x; ++x)
        {
            for (int y = minIndex.y; y <= maxIndex.y; ++y)
            {
                for (int z = minIndex.z; z <= maxIndex.z; ++z)
                {
                    glm::ivec3 index(x, y, z);
                    if (IsValidIndex(index))
                    {
                        occupiedCells.push_back(index);
                    }
                }
            }
        }
        return occupiedCells;
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
        glm::vec3 size = collider->GetBoundingBox();

        std::vector<glm::ivec3> occupiedCells = GetOccupiedCells(position, size);
        for (const auto& index : occupiedCells)
        {
            grid[index.x][index.y][index.z].push_back(collider);
        }
    }


    void SpatialPartitioning::RemoveCollider(Collider* collider)
    {
        std::cout << "Removing collider from spatial partitioning" << std::endl;
        std::cout <<  collider->GetOwner() << std::endl;
        glm::vec3 position = collider->GetTransform()->GetPositionWorldSpace();
        glm::vec3 size = collider->GetBoundingBox();

        std::vector<glm::ivec3> occupiedCells = GetOccupiedCells(position, size);
        for (const auto& index : occupiedCells)
        {
            auto& cell = grid[index.x][index.y][index.z];
            cell.erase(std::remove(cell.begin(), cell.end(), collider), cell.end());
        }
    }

    std::vector<Collider*> SpatialPartitioning::GetPotentialCollisions(Collider* collider)
    {
        glm::vec3 position = collider->GetTransform()->GetPositionWorldSpace();
        glm::vec3 size = collider->GetBoundingBox();

        std::vector<glm::ivec3> occupiedCells = GetOccupiedCells(position, size);
        std::vector<Collider*> potentialCollisions;

        for (const auto& index : occupiedCells)
        {
            const auto& cell = grid[index.x][index.y][index.z];
            potentialCollisions.insert(potentialCollisions.end(), cell.begin(), cell.end());
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
