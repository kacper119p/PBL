#include "Graph.h"

namespace Engine
{
    void Graph::AddNode(int Id, const glm::vec3 Position)
    {
        Nodes.emplace(Id, Node(Id, Position));
    }

    void Graph::AddConnection(const int FromId, const int ToId)
    {
        auto fromNode = Nodes.find(FromId);
        auto toNode = Nodes.find(ToId);

        if (fromNode == Nodes.end() || toNode == Nodes.end())
        {
            return;
        }

        fromNode->second.AddNeighbor(ToId);
        toNode->second.AddNeighbor(FromId);
    }

    bool Graph::AreConnected(int FromId, int ToId) const
    {
        auto it = Nodes.find(FromId);
        if (it == Nodes.end())
            return false;

        const auto& connections = it->second.GetNeighbors();
        return std::find(connections.begin(), connections.end(), ToId) != connections.end();
    }

    void Graph::RemoveNode(int Id)
    {
        for (auto& [otherId, node] : Nodes)
        {
            auto& neighbors = const_cast<std::vector<int>&>(node.GetNeighbors());
            neighbors.erase(
                    std::remove(neighbors.begin(), neighbors.end(), Id),
                    neighbors.end()
                    );
        }

        Nodes.erase(Id);
    }

    const Node* Graph::GetNode(const int Id) const
    {
        if (Id < 0 || Id >= static_cast<int>(Nodes.size()))
            return nullptr;
        return &Nodes.at(Id);
    }

    const std::unordered_map<int, Node>& Graph::GetAllNodes() const
    {
        return Nodes;
    }
}
