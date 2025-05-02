#pragma once

#include "glm/glm.hpp"
#include <vector>

namespace Engine
{
    /**
     * @brief Represents a node in a graph structure.
     */
    class Node
    {
    public:
        /**
         * @brief Default constructor.
         */
        Node() = default;

        /**
         * @brief Constructs a node with specified ID and position.
         * @param Id Unique identifier.
         * @param Position 2D position in world space.
         */
        Node(int Id, const glm::vec2& Position);

        /**
         * @brief Gets the unique identifier of the node.
         * @return The node's ID.
         */
        int GetId() const;

        /**
         * @brief Gets the 2D position of the node in world space.
         * @return Reference to the position vector.
         */
        const glm::vec2& GetPosition() const;

        /**
         * @brief Gets the list of connected neighbor node IDs.
         * @return Constant reference to the neighbors vector.
         */
        const std::vector<int>& GetNeighbors() const;

        /**
         * @brief Sets the unique identifier of the node.
         * @param NewId New ID to assign.
         */
        void SetId(int NewId);

        /**
         * @brief Sets the 2D position of the node in world space.
         * @param NewPosition New position to assign.
         */
        void SetPosition(const glm::vec2& NewPosition);

        /**
         * @brief Adds a neighbor node ID to the list of neighbors.
         * @param NeighborId ID of the neighbor node to add.
         */
        void AddNeighbor(int NeighborId);

        /**
         * @brief Clears all neighbor connections from the node.
         */
        void ClearNeighbors();

    private:
        int Id{}; ///< Unique identifier of the node.
        glm::vec2 Position{}; ///< 2D position of the node in world space.
        std::vector<int> Neighbors; ///< List of connected neighbor node IDs.
    };
}
