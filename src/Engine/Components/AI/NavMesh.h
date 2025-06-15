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
         * @param Padding Distance from edges of NavMesh.
         */
        void BuildNavMesh(Entity* Root, float Spacing = 1.0f, float Padding = 1.0f);

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
         * @brief Spacing getter.
         * @return Spacing of NavMesh nodes.
         */
        [[nodiscard]] float GetSpacing() const
        {
            return Spacing;
        }

        /**
         * @brief Spacing setter.
         */
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

        /**
         * @brief Returns all models and their associated transforms used in the navigation mesh.
         * @return Vector of model-transform pairs.
         */
        [[nodiscard]] const std::vector<std::pair<Models::Model*, glm::mat4>>& GetModelTransforms() const
        {
            return ModelTransforms;
        }

        /**
         * @brief Checks whether a position lies on or near the navmesh.
         * @param Position World position to check.
         * @param MaxDistance Maximum allowed distance from the navmesh.
         * @return True if position is on the navmesh; false otherwise.
         */
        bool IsOnNavMesh(const glm::vec3& Position, float MaxDistance);

        /**
         * @brief Sets padding distance used when generating the navmesh.
         * @param Pad Padding value.
         */
        void SetPadding(float Pad) { Padding = Pad; }

        /**
         * @brief Gets current padding value.
         * @return Padding used during navmesh generation.
         */
        [[nodiscard]] float GetPadding() const { return Padding; }

    private:
        float Padding = 1.0f; ///< Padding around obstacles.
        std::unique_ptr<Graph> NavGraph = std::make_unique<Graph>(); ///< The navigation graph.
        float Spacing = 1.0f; ///< Spacing between NavMesh nodes.
        std::vector<std::pair<Models::Model*, glm::mat4>> ModelTransforms;
        ///< Models and transforms used in navmesh calculation.

        /**
         * @brief Collects model data and computes blocked areas for the navmesh.
         * @param Root Scene root.
         * @param ModelTransforms Output vector for model-transform pairs.
         * @param BlockedAreas Output vector for blocked AABB areas in 2D.
         * @param SceneMin Output minimum 2D bounds of the scene.
         * @param SceneMax Output maximum 2D bounds of the scene.
         * @param LargestModelSize Output size of the largest model.
         */
        void CollectModelData(Entity* Root,
                              std::vector<std::pair<Models::Model*, glm::mat4>>& ModelTransforms,
                              std::vector<std::pair<glm::vec2, glm::vec2>>& BlockedAreas,
                              glm::vec2& SceneMin,
                              glm::vec2& SceneMax,
                              float& LargestModelSize);

        /**
         * @brief Adds a blocked area based on a model's geometry.
         * @param Model The model to analyze.
         * @param Transform Transform matrix of the model.
         * @param BlockedAreas Vector to add resulting blocked areas to.
         */
        void AddBlockedAreaFromModel(Models::Model* Model, const glm::mat4& Transform,
                                     std::vector<std::pair<glm::vec2, glm::vec2>>& BlockedAreas);

        /**
         * @brief Updates scene bounding box to include given model.
         * @param Model The model to analyze.
         * @param Transform Transform matrix of the model.
         * @param SceneMin Minimum 2D bounds of the scene.
         * @param SceneMax Maximum 2D bounds of the scene.
         * @param LargestModelSize Variable to track the largest model size encountered.
         */
        void UpdateSceneBoundsFromModel(Models::Model* Model, const glm::mat4& Transform,
                                        glm::vec2& SceneMin, glm::vec2& SceneMax,
                                        float& LargestModelSize);

        /**
         * @brief Generates the navigation grid based on provided geometry and spacing.
         * @param ModelTransforms Models used in the scene.
         * @param BlockedAreas Areas considered not walkable.
         * @param SceneMin Minimum boundary of the navmesh area.
         * @param SceneMax Maximum boundary of the navmesh area.
         * @param Spacing Distance between nodes.
         * @param Padding Padding around obstacles.
         */
        void GenerateNavigationGrid(const std::vector<std::pair<Models::Model*, glm::mat4>>& ModelTransforms,
                                    const std::vector<std::pair<glm::vec2, glm::vec2>>& BlockedAreas,
                                    const glm::vec2& SceneMin,
                                    const glm::vec2& SceneMax,
                                    float Spacing,
                                    float Padding);

        /**
         * @brief Checks if a point lies inside any of the blocked areas.
         * @param Point Point to test.
         * @param BlockedAreas Areas that block movement.
         * @return True if point is inside any blocked area.
         */
        [[nodiscard]] bool IsPointBlocked(const glm::vec2& Point,
                                          const std::vector<std::pair<glm::vec2, glm::vec2>>& BlockedAreas) const;

        /**
         * @brief Finds the closest hit point on surface geometry from an origin point.
         * @param Origin Ray origin.
         * @param ModelTransforms Models to check for intersection.
         * @param OutHitPoint Output closest hit position.
         * @return True if a hit was found.
         */
        bool FindClosestSurfaceHit(const glm::vec3& Origin,
                                   const std::vector<std::pair<Models::Model*, glm::mat4>>& ModelTransforms,
                                   glm::vec3& OutHitPoint) const;
    };
}
