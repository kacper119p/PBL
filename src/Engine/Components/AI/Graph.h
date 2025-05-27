#pragma once

#include <unordered_map>
#include "Node.h"

namespace Engine
{
    /**
     * @brief Represents a simple 3D graph consisting of nodes and their connections.
     */
    class Graph
    {
    public:
        /**
         * @brief Adds a new node to the graph.
         * @param Id Unique identifier for the node.
         * @param Position 3D position of the node.
         */
        void AddNode(int Id, glm::vec3 Position);

        /**
         * @brief Creates a directed connection between two existing nodes.
         * @param FromId ID of the starting node.
         * @param ToId ID of the destination node.
         */
        void AddConnection(int FromId, int ToId);

        /**
         * @brief Checks if two nodes are directly connected.
         * @param FromId Source node ID.
         * @param ToId Destination node ID.
         * @return True if connected, false otherwise.
         */
        bool AreConnected(int FromId, int ToId) const;

        /**
         * @brief Removes node from the graph.
         * @param Id ID of the node to be removed.
         */
        void RemoveNode(int Id);

        /**
         * @brief Retrieves a node by its ID.
         * @param Id The ID of the node.
         * @return Const reference to the node.
         */
        [[nodiscard]] const Node& GetNode(int Id) const;

        /**
         * @brief Returns all nodes stored in the graph.
         * @return Const reference to a map of all nodes.
         */
        [[nodiscard]] const std::unordered_map<int, Node>& GetAllNodes() const;

    private:
        std::unordered_map<int, Node> Nodes; ///< Map of all nodes in the graph.
    };
}
