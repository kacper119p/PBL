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

        void RemovePaddingNodes();

        const std::vector<std::pair<Models::Model*, glm::mat4>>& GetModelTransforms() const
        {
            return ModelTransforms;
        }

        bool IsOnNavMesh(const glm::vec3& Position, float MaxDistance);

        void SetPadding(float Pad) { Padding = Pad; }
        [[nodiscard]] float GetPadding() const { return Padding; }

    private:
        float Padding = 1.0f;
        std::unique_ptr<Graph> NavGraph = std::make_unique<Graph>(); ///< The navigation graph.
        float Spacing = 1.0f; ///< Spacing between NavMesh nodes.
        std::vector<std::pair<Models::Model*, glm::mat4>> ModelTransforms;
    };
}
