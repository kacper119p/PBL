#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Engine
{
    class Collider;

    class SpatialPartitioning
    {
    public:
        static SpatialPartitioning& GetInstance();
        static void DestroyInstance();

        void AddCollider(Collider* collider);
        void RemoveCollider(Collider* collider);
        std::vector<Collider*> GetPotentialCollisions(Collider* collider);
        void SetCellSize(float newCellSize);
        std::vector<Collider*> QuerySphere(glm::vec3& position, float radius) const;

    private:
        SpatialPartitioning();
        ~SpatialPartitioning();

        SpatialPartitioning(const SpatialPartitioning&) = delete;
        SpatialPartitioning& operator=(const SpatialPartitioning&) = delete;

        glm::ivec2 GetCellIndex(const glm::vec3& position) const;
        std::vector<glm::ivec2> GetOccupiedCells(const glm::vec3& position, const glm::vec3& size) const;
        bool IsValidIndex(const glm::ivec2& index) const;
        int GetOverflowRegion(const glm::ivec2& index) const;

        static SpatialPartitioning* instance_;

        float cellSize;
        glm::ivec2 gridDimensions;
        glm::vec3 origin;

        std::vector<std::vector<std::vector<Collider*>>> grid;

        enum OverflowRegion
        {
            TOP_LEFT = 0,
            TOP,
            TOP_RIGHT,
            RIGHT,
            BOTTOM_RIGHT,
            BOTTOM,
            BOTTOM_LEFT,
            LEFT,
            OVERFLOW_COUNT
        };

        std::vector<Collider*> overflowCells[OVERFLOW_COUNT];
    };

} // namespace Engine
