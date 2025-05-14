#pragma once

#include "Engine/Components/AI/AStar.h"
#include "NavArea.h"

namespace Engine
{
    /**
     * @brief Manages the navigation mesh for pathfinding and AI navigation.
     */
    class NavMesh
    {
    public:
        /**
         * @brief Gets the singleton instance of the NavMesh.
         * @return Reference to the NavMesh instance.
         */
        static NavMesh& Get();

        /**
         * @brief Gets the navigation graph.
         * @return Pointer to the navigation graph.
         */
        Graph* GetGraph();

        /**
         * @brief Builds the navigation mesh from the scene root.
         * @param Root Scene root entity.
         * @param Spacing Distance between nodes.
         */
        void BuildNavMesh(Entity* Root, float Spacing = 1.0f);

        /**
         * @brief Sets NavGraph to nullptr.
         */
        void ClearGraph();

        /**
         * @brief Builds and assigns the navigation mesh from the given root entity.
         * @param Root Scene root entity.
         */
        void BakeNavMesh(Entity* Root);

        /**
         * @brief Removes all nodes that have NavArea component and IsWalkable is false from the existing NavMesh.
         * @param Root Scene root entity.
         */
        void RemoveNotWalkableNodes(Entity* Root);

        /**
         * @brief Tests if a ray intersects a triangle.
         * @param RayOrigin The origin of the ray.
         * @param RayDirection The direction of the ray.
         * @param Vertex0, Vertex1, Vertex2 Triangle vertices.
         * @param HitPoint The intersection point, if any.
         * @return True if the ray intersects the triangle, otherwise false.
         */
        bool RayIntersectsTriangle(const glm::vec3& RayOrigin, const glm::vec3& RayDirection,
                                   const glm::vec3& Vertex0, const glm::vec3& Vertex1, const glm::vec3& Vertex2,
                                   glm::vec3* HitPoint = nullptr);

        [[nodiscard]] float GetSpacing() const
        {
            return Spacing;
        }

        void SetSpacing(float Spacing)
        {
            this->Spacing = Spacing;
        }

        /**
         * @brief Maps a 3D world position to a corresponding node ID in the graph.
         * @param Position 3D world position.
         * @return Node ID closest to the given position.
         */
        [[nodiscard]] int GetNodeIdFromPosition(const glm::vec3& Position) const;

    private:
        std::unique_ptr<Graph> NavGraph = std::make_unique<Graph>(); ///< The navigation graph.
        float Spacing = 1.0f; ///< Spacing between NavMesh nodes.
    };
}
