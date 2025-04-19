#pragma once

#include <unordered_map>
#include <vector>
#include "Collider.h"
#include <functional>
#include "glm/glm.hpp"
#include "../Interfaces/IUpdateable.h"

namespace std
{
    template<>
    struct hash<glm::ivec3>
    {
        size_t operator()(const glm::ivec3& vec) const
        {
            return hash<int>()(vec.x) ^ (hash<int>()(vec.y) << 1) ^ (hash<int>()(vec.z) << 2);
        }
    };
} // namespace std

namespace Engine
{
    class SpatialPartitioning : public IUpdateable
    {
    private:
        float cellSize;
        std::unordered_map<glm::ivec3, std::vector<Collider*>> grid;

        glm::ivec3 GetCellIndex(const glm::vec3& position) const;

    public:
        explicit SpatialPartitioning(float cellSize);

        void AddCollider(Collider* collider);
        void RemoveCollider(Collider* collider);
        std::vector<Collider*> GetPotentialCollisions(Collider* collider);

        void Update(float deltaTime) override;

    };
} // namespace Engine
