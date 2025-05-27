#pragma once

#include "Engine/Components/Updateable.h"
#include "Engine/EngineObjects/Entity.h"
#include "glm/glm.hpp"
#include "Graph.h"
#include "Engine/EngineObjects/Scene/SceneManager.h"

namespace Engine
{
    /**
     * @brief A* pathfinding component using a navigation graph.
     */
    class AStar final
    {
    public:
        /**
         * @brief Constructs a new AStar component.
         */
        AStar();

        /**
         * @brief Destructs the AStar component.
         */
        ~AStar();

        /**
         * @brief Sets the navigation graph used for pathfinding.
         * @param NewGraph Pointer to the navigation graph.
         */
        void SetGraph(const Graph* NewGraph);

        /**
         * @brief Calculates a path from the selected entity to the given goal using A*.
         * @param GoalPosition World-space destination position.
         */
        void ComputePath(const glm::vec3& GoalPosition, Entity* Entity);

        /**
         * @brief Finds the path between two given node IDs.
         * @param StartId Starting node ID.
         * @param GoalId Goal node ID.
         */
        void FindPath(int StartId, int GoalId);

        /**
         * @brief Returns the current computed path.
         * @return Vector of node IDs forming the path.
         */
        [[nodiscard]] std::vector<int> GetPath() const;

        void ClearPath(Entity* Entity)
        {
            Path.clear();
        }

        /**
         * @brief Sets the start position in world space.
         * @param Position 3D world position.
         */
        void SetStartPosition(const glm::vec3& Position);

        /**
         * @brief Sets the goal position in world space.
         * @param Position 3D world position.
         */
        void SetGoalPosition(const glm::vec3& Position);

        [[nodiscard]] float GetMoveSpeed() const;

        void SetMoveSpeed(float Speed);

        [[nodiscard]] glm::vec3 GetGoalPosition() const;

        [[nodiscard]] int GetCurrentPathIndex() const;

        void SetCurrentPathIndex(int Index);

        void UpdateMovement(float DeltaTime, Entity* Entity);

        bool IsPathFinished() const
        {
            return Path.empty() || CurrentPathIndex >= Path.size();
        }

        void SetMovementEnabled(bool Enabled)
        {
            MovementEnabled = Enabled;
        }

        void SmoothPath();

        bool IsLineWalkable(int FromId, int ToId);

    private:
        const Graph* NavGraph = nullptr; ///< Pointer to the navigation graph.

        glm::vec2 ObjectPosition{}; ///< Current 2D position of the controlled object.
        glm::vec2 CurrentDirection = {1.0f, 0.0f}; ///< Current direction of movement.
        float MoveSpeed = 10.0f; ///< Speed at which the object moves.

        std::vector<int> Path; ///< Sequence of node IDs representing the current path.
        int CurrentPathIndex = 0; ///< Index of the next node in the path.
        int StartId = -1; ///< ID of the start node.
        int GoalId = -1; ///< ID of the goal node.

        bool MovementEnabled = true;

        glm::vec3 StartPosition{}; ///< The starting position of the A* path.
        glm::vec3 GoalPosition{}; ///< The end position of the A* path.

        /**
         * @brief Internal record used during A* pathfinding.
         */
        struct NodeRecord
        {
            int NodeId; ///< ID of the node.
            float CostSoFar; ///< Cost from the start to this node.
            float EstimatedTotalCost; ///< Estimated total cost (CostSoFar + heuristic).
            int CameFrom; ///< ID of the previous node in the path.

            bool operator>(const NodeRecord& Other) const
            {
                return EstimatedTotalCost > Other.EstimatedTotalCost;
            }
        };
    };

}
