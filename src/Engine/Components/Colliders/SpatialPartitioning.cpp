#include "SpatialPartitioning.h"
#include <algorithm>
#include <cmath>
#include <spdlog/spdlog.h>
#include <unordered_set>
#include "Collider.h"

namespace Engine
{

    SpatialPartitioning* SpatialPartitioning::instance_ = nullptr;

    SpatialPartitioning::SpatialPartitioning() : cellSize(2.0f), origin(glm::vec2(-100.0f, -100.0f)) {}

    SpatialPartitioning::~SpatialPartitioning() {}

    SpatialPartitioning& SpatialPartitioning::GetInstance()
    {
        if (!instance_)
            instance_ = new SpatialPartitioning();
        return *instance_;
    }

    void SpatialPartitioning::DestroyInstance()
    {
        delete instance_;
        instance_ = nullptr;
    }

    glm::ivec2 SpatialPartitioning::GetCellIndex(const glm::vec3& position) const
    {
        glm::vec2 localPos = glm::vec2(position.x, position.z);
        return glm::ivec2(static_cast<int>(std::floor(localPos.x / cellSize)),
                          static_cast<int>(std::floor(localPos.y / cellSize)));
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

    void SpatialPartitioning::AddCollider(Collider* collider)
    {
        if (!collider || !collider->GetTransform())
            return;

        glm::vec3 position = collider->GetTransform()->GetPositionWorldSpace();
        glm::vec3 size = collider->GetBoundingBox();

        std::vector<glm::ivec2> occupiedCells = GetOccupiedCells(position, size);

        for (const auto& index : occupiedCells)
            grid[index].push_back(collider);

        colliderCellMap[collider] = occupiedCells;
    }

    void SpatialPartitioning::RemoveCollider(Collider* collider)
    {
        if (!collider)
            return;

        auto it = colliderCellMap.find(collider);
        if (it == colliderCellMap.end())
            return;

        for (const auto& index : it->second)
        {
            auto gridIt = grid.find(index);
            if (gridIt == grid.end())
                continue;

            auto& cell = gridIt->second;
            cell.erase(std::remove(cell.begin(), cell.end(), collider), cell.end());

            if (cell.empty())
                grid.erase(index);
        }

        colliderCellMap.erase(collider);
    }

    std::vector<Collider*> SpatialPartitioning::GetPotentialCollisions(Collider* collider)
    {
        if (!collider || !collider->GetTransform())
            return {};

        glm::vec3 position = collider->GetTransform()->GetPositionWorldSpace();
        glm::vec3 size = collider->GetBoundingBox();

        std::vector<glm::ivec2> occupiedCells = GetOccupiedCells(position, size);
        std::unordered_set<Collider*> unique;
        std::vector<Collider*> potentialCollisions;

        for (const auto& index : occupiedCells)
        {
            for (int dx = -1; dx <= 1; ++dx)
            {
                for (int dy = -1; dy <= 1; ++dy)
                {
                    glm::ivec2 neighborIndex = glm::ivec2(index.x + dx, index.y + dy);
                    const auto& cell = grid[neighborIndex];

                    for (Collider* other : cell)
                    {
                        if (other && other != collider && other->GetTransform() && unique.insert(other).second)
                        {
                            potentialCollisions.push_back(other);
                        }
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

        std::vector<Collider*> allColliders;
        for (auto& [index, cell] : grid)
        {
            allColliders.insert(allColliders.end(), cell.begin(), cell.end());
        }

        cellSize = newCellSize;
        grid.clear();
        colliderCellMap.clear();

        for (Collider* collider : allColliders)
        {
            AddCollider(collider);
        }

        spdlog::info("Grid updated with new cell size: {}", cellSize);
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
                auto it = grid.find(index);
                if (it != grid.end())
                {
                    for (Collider* collider : it->second)
                    {
                        if (!collider || !collider->GetTransform() || uniqueColliders.count(collider))
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

        return result;
    }

    void SpatialPartitioning::ValidateGrid()
    {
        int totalInvalid = 0;
        for (auto& [index, cell] : grid)
        {
            auto& vec = cell;
            vec.erase(std::remove_if(vec.begin(), vec.end(),
                                     [&](Collider* c)
                                     {
                                         bool bad = !c || !c->GetTransform();
                                         if (bad)
                                             totalInvalid++;
                                         return bad;
                                     }),
                      vec.end());
        }

        if (totalInvalid > 0)
            spdlog::warn("Removed {} invalid collider pointers from the grid during validation.", totalInvalid);
    }

} // namespace Engine
