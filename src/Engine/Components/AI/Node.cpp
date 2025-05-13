#include "Node.h"

namespace Engine
{
    Node::Node(int Id, const glm::vec3& Position) :
        Id(Id), Position(Position)
    {
    }

    int Node::GetId() const
    {
        return Id;
    }

    const glm::vec3& Node::GetPosition() const
    {
        return Position;
    }

    const std::vector<int>& Node::GetNeighbors() const
    {
        return Neighbors;
    }

    void Node::SetId(int NewId)
    {
        Id = NewId;
    }

    void Node::SetPosition(const glm::vec3& NewPosition)
    {
        Position = NewPosition;
    }

    void Node::AddNeighbor(int NeighborId)
    {
        Neighbors.push_back(NeighborId);
    }

    void Node::ClearNeighbors()
    {
        Neighbors.clear();
    }
}
