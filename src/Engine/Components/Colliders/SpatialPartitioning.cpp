#include "SpatialPartitioning.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <spdlog/spdlog.h>
#include <unordered_set>
#include "Collider.h"
namespace Engine
{
    SpatialPartitioning* SpatialPartitioning::instance_ = nullptr;

    SpatialPartitioning::SpatialPartitioning() :
        cellSize(3.0f), gridDimensions(glm::ivec2(10, 10)), origin(glm::vec3(-10.0f, -10.0f, -10.0f))
    {
        grid.resize(gridDimensions.x);
        for (int x = 0; x < gridDimensions.x; ++x)
        {
            grid[x].resize(gridDimensions.y);
        }

        for (int i = 0; i < OVERFLOW_COUNT; ++i)
        {
            overflowCells[i] = std::vector<Collider*>();
        }
    }

    SpatialPartitioning::~SpatialPartitioning() {}

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
        if (instance_)
        {
            delete instance_;
            instance_ = nullptr;
        }
    }

    glm::ivec2 SpatialPartitioning::GetCellIndex(const glm::vec3& position) const
    {
        glm::vec3 localPos = position - origin;
        return glm::ivec2(static_cast<int>(std::floor(localPos.x / cellSize)),
                          static_cast<int>(std::floor(localPos.z / cellSize)));
    }

    std::vector<glm::ivec2> SpatialPartitioning::GetOccupiedCells(const glm::vec3& position,
                                                                  const glm::vec3& size) const
    {
        glm::vec3 minBounds = position - size * 0.5f;
        glm::vec3 maxBounds = position + size * 0.5f;

        glm::ivec2 minIndex = GetCellIndex(minBounds);
        glm::ivec2 maxIndex = GetCellIndex(maxBounds);

        std::vector<glm::ivec2> occupiedCells;
        for (int x = minIndex.x; x <= maxIndex.x; ++x)
        {
            for (int y = minIndex.y; y <= maxIndex.y; ++y)
            {
                occupiedCells.emplace_back(x, y);
            }
        }
        return occupiedCells;
    }

    bool SpatialPartitioning::IsValidIndex(const glm::ivec2& index) const
    {
        return index.x >= 0 && index.x < gridDimensions.x && index.y >= 0 && index.y < gridDimensions.y;
    }

    int SpatialPartitioning::GetOverflowRegion(const glm::ivec2& index) const
    {
        bool left = index.x < 0;
        bool right = index.x >= gridDimensions.x;
        bool top = index.y >= gridDimensions.y;
        bool bottom = index.y < 0;

        if (left && top)
            return TOP_LEFT;
        if (right && top)
            return TOP_RIGHT;
        if (left && bottom)
            return BOTTOM_LEFT;
        if (right && bottom)
            return BOTTOM_RIGHT;
        if (left)
            return LEFT;
        if (right)
            return RIGHT;
        if (top)
            return TOP;
        if (bottom)
            return BOTTOM;

        return -1;
    }

    void SpatialPartitioning::AddCollider(Collider* collider)
    {
        glm::vec3 position = collider->GetTransform()->GetPositionWorldSpace();
        glm::vec3 size = collider->GetBoundingBox();

        std::vector<glm::ivec2> occupiedCells = GetOccupiedCells(position, size);
        for (const auto& index : occupiedCells)
        {
            if (IsValidIndex(index))
            {
                grid[index.x][index.y].push_back(collider);
            }
            else
            {
                int region = GetOverflowRegion(index);
                if (region >= 0 && region < OVERFLOW_COUNT)
                {
                    overflowCells[region].push_back(collider);
                }
            }
        }
    }

    void SpatialPartitioning::RemoveCollider(Collider* collider)
    {
        std::cout << "Removing collider from spatial partitioning" << std::endl;
        std::cout << collider->GetOwner() << std::endl;

        glm::vec3 position = collider->GetTransform()->GetPositionWorldSpace();
        glm::vec3 size = collider->GetBoundingBox();

        std::vector<glm::ivec2> occupiedCells = GetOccupiedCells(position, size);
        for (const auto& index : occupiedCells)
        {
            if (IsValidIndex(index))
            {
                auto& cell = grid[index.x][index.y];
                cell.erase(std::remove(cell.begin(), cell.end(), collider), cell.end());
            }
            else
            {
                int region = GetOverflowRegion(index);
                if (region >= 0 && region < OVERFLOW_COUNT)
                {
                    auto& cell = overflowCells[region];
                    cell.erase(std::remove(cell.begin(), cell.end(), collider), cell.end());
                }
            }
        }
    }

    std::vector<Collider*> SpatialPartitioning::GetPotentialCollisions(Collider* collider)
    {
        glm::vec3 position = collider->GetTransform()->GetPositionWorldSpace();
        glm::vec3 size = collider->GetBoundingBox();

        std::vector<glm::ivec2> occupiedCells = GetOccupiedCells(position, size);
        std::vector<Collider*> potentialCollisions;

        for (const auto& index : occupiedCells)
        {
            if (IsValidIndex(index))
            {
                const auto& cell = grid[index.x][index.y];
                potentialCollisions.insert(potentialCollisions.end(), cell.begin(), cell.end());
            }
            else
            {
                int region = GetOverflowRegion(index);
                if (region >= 0 && region < OVERFLOW_COUNT)
                {
                    const auto& cell = overflowCells[region];
                    potentialCollisions.insert(potentialCollisions.end(), cell.begin(), cell.end());
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

        glm::vec3 maxPosition = origin + glm::vec3(gridDimensions.x, 0, gridDimensions.y) * cellSize;
        gridDimensions = glm::ivec2(static_cast<int>(std::ceil((maxPosition.x - origin.x) / cellSize)),
                                    static_cast<int>(std::ceil((maxPosition.z - origin.z) / cellSize)));

        std::vector<std::vector<std::vector<Collider*>>> newGrid;
        newGrid.resize(gridDimensions.x);
        for (int x = 0; x < gridDimensions.x; ++x)
        {
            newGrid[x].resize(gridDimensions.y);
        }

        for (int x = 0; x < grid.size(); ++x)
        {
            for (int y = 0; y < grid[x].size(); ++y)
            {
                for (Collider* collider : grid[x][y])
                {
                    glm::vec3 position = collider->GetTransform()->GetPositionWorldSpace();
                    glm::ivec2 newIndex = GetCellIndex(position);

                    if (IsValidIndex(newIndex))
                    {
                        newGrid[newIndex.x][newIndex.y].push_back(collider);
                    }
                    else
                    {
                        int region = GetOverflowRegion(newIndex);
                        if (region >= 0 && region < OVERFLOW_COUNT)
                        {
                            overflowCells[region].push_back(collider);
                        }
                    }
                }
            }
        }

        grid = std::move(newGrid);

        spdlog::info("Grid resized. New cell size: {}, New dimensions: ({}, {})", cellSize, gridDimensions.x,
                     gridDimensions.y);
    }

    std::vector<Collider*> SpatialPartitioning::QuerySphere(glm::vec3& position, float radius) const
    {
        std::vector<Collider*> result;
        std::unordered_set<Collider*> uniqueColliders;

        glm::vec3 minBounds = position - glm::vec3(radius);
        glm::vec3 maxBounds = position + glm::vec3(radius);

        glm::ivec2 minIndex = GetCellIndex(minBounds);
        glm::ivec2 maxIndex = GetCellIndex(maxBounds);

        for (int x = minIndex.x; x <= maxIndex.x; ++x)
        {
            for (int y = minIndex.y; y <= maxIndex.y; ++y)
            {
                glm::ivec2 index(x, y);
                if (IsValidIndex(index))
                {
                    const auto& cell = grid[x][y];
                    for (Collider* collider : cell)
                    {
                        if (!collider || uniqueColliders.count(collider))
                            continue;

                        glm::vec3 colliderPos = collider->GetTransform()->GetPositionWorldSpace();
                        float approxRange = glm::length(collider->GetBoundingBox()) * 0.5f;
                        float totalRange = radius + approxRange;

                        if (glm::distance2(position, colliderPos) <= totalRange * totalRange)
                        {
                            result.push_back(collider);
                            uniqueColliders.insert(collider);
                        }
                    }
                }
            }
        }

        // Przeszukaj overflow cellki
        for (int i = 0; i < OVERFLOW_COUNT; ++i)
        {
            for (Collider* collider : overflowCells[i])
            {
                if (!collider || uniqueColliders.count(collider))
                    continue;

                glm::vec3 colliderPos = collider->GetTransform()->GetPositionWorldSpace();
                float approxRange = glm::length(collider->GetBoundingBox()) * 0.5f;
                float totalRange = radius + approxRange;

                if (glm::distance2(position, colliderPos) <= totalRange * totalRange)
                {
                    result.push_back(collider);
                    uniqueColliders.insert(collider);
                }
            }
        }

        return result;
    }

} // namespace Engine
