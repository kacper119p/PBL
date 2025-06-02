#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

namespace Engine
{
    class Collider;

    struct Vec2Hash
    {
        std::size_t operator()(const glm::ivec2& v) const noexcept
        {
            std::hash<int> hasher;
            return hasher(v.x) ^ (hasher(v.y) << 1);
        }
    };

    class SpatialPartitioning
    {
    public:
        static SpatialPartitioning& GetInstance();
        static void DestroyInstance();
        glm::ivec2 GetCellIndex(const glm::vec3& position) const;

        void AddCollider(Collider* collider);
        void RemoveCollider(Collider* collider);
        std::vector<Collider*> GetPotentialCollisions(Collider* collider);
        std::vector<Collider*> QuerySphere(glm::vec3& position, float radius) const;

        void SetCellSize(float newCellSize);

    private:
        SpatialPartitioning();
        ~SpatialPartitioning();

        void ValidateGrid();
       
        std::vector<glm::ivec2> GetOccupiedCells(const glm::vec3& position, const glm::vec3& size) const;

        float cellSize;
        glm::vec2 origin;

        std::unordered_map<glm::ivec2, std::vector<Collider*>, Vec2Hash> grid;
        std::unordered_map<Collider*, std::vector<glm::ivec2>> colliderCellMap;

        static SpatialPartitioning* instance_;
    };
} // namespace Engine
