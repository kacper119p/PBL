#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "Collider.h"

namespace Engine
{
    class SpatialPartitioning
    {
    private:
        float cellSize;
        glm::ivec3 gridDimensions;
        glm::vec3 origin;
        std::vector<std::vector<std::vector<std::vector<Collider*>>>> grid;
        static SpatialPartitioning* instance_;

        SpatialPartitioning();
        ~SpatialPartitioning(); 

    public:
        SpatialPartitioning(const SpatialPartitioning&) = delete;
        SpatialPartitioning& operator=(const SpatialPartitioning&) = delete;
        static SpatialPartitioning& GetInstance();
        static void DestroyInstance();
        glm::ivec3 GetCellIndex(const glm::vec3& position) const;
        bool IsValidIndex(const glm::ivec3& index) const;
        void AddCollider(Collider* collider);
        void RemoveCollider(Collider* collider);
        std::vector<Collider*> GetPotentialCollisions(Collider* collider);
        void SetCellSize(float newCellSize);
    };
} // namespace Engine
