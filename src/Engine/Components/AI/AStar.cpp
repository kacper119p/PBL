#include "AStar.h"
#include <queue>
#include "NavMesh.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/EngineObjects/UpdateManager.h"
#include "Serialization/SerializationUtility.h"
#include "spdlog/spdlog.h"

namespace Engine
{
    AStar::AStar()
    {
        UpdateManager::GetInstance()->RegisterComponent(this);
    }

    AStar::~AStar()
    {
        UpdateManager::GetInstance()->UnregisterComponent(this);
    }

#if EDITOR
    void AStar::DrawImGui()
    {
        if (ImGui::CollapsingHeader("A* Pathfinding", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputFloat("Move Speed", &MoveSpeed);
            float spacing = NavMesh::Get().GetSpacing();
            if (ImGui::InputFloat("Spacing", &spacing))
            {
                NavMesh::Get().SetSpacing(spacing);
            }
            ImGui::InputFloat3("Goal", &GoalPosition.x);

            if (ImGui::Button("Bake NavMesh"))
            {
                NavMesh::Get().BakeNavMesh(GetOwner()->GetScene()->GetRoot());
                SetGraph(NavMesh::Get().GetGraph());
            }

            if (ImGui::Button("Compute Path"))
            {
                ComputePath(GoalPosition);
            }

            if (!Path.empty())
            {
                ImGui::Text("Path Nodes: %zu", Path.size());
                ImGui::Text("Current Node Index: %d", CurrentPathIndex);

                if (ImGui::TreeNode("Path List"))
                {
                    for (size_t i = 0; i < Path.size(); ++i)
                    {
                        ImGui::BulletText("Node ID: %d", Path[i]);
                    }
                    ImGui::TreePop();
                }
            }
            else
            {
                ImGui::Text("No active path.");
            }
        }
    }
#endif

    void AStar::FindPath(int StartId, int GoalId)
    {
        if (!NavGraph)
        {
            Path.clear();
            return;
        }

        std::priority_queue<NodeRecord, std::vector<NodeRecord>, std::greater<>> openList;
        std::unordered_map<int, NodeRecord> allNodes;
        std::unordered_map<int, bool> closedList;

        const glm::vec2& goalPos = NavGraph->GetNode(GoalId).GetPosition();
        const glm::vec2& startPos = NavGraph->GetNode(StartId).GetPosition();

        const NodeRecord startRecord{StartId, 0.0f, glm::distance(startPos, goalPos), -1};
        openList.push(startRecord);
        allNodes[StartId] = startRecord;

        while (!openList.empty())
        {
            NodeRecord current = openList.top();
            openList.pop();

            if (current.NodeId == GoalId)
            {
                std::vector<int> path;
                int node = GoalId;
                while (node != -1)
                {
                    path.push_back(node);
                    node = allNodes[node].CameFrom;
                }
                std::reverse(path.begin(), path.end());
                Path = path;
                return;
            }

            closedList[current.NodeId] = true;

            for (int neighborId : NavGraph->GetNode(current.NodeId).GetNeighbors())
            {
                if (closedList.count(neighborId))
                    continue;

                const float g = current.CostSoFar + glm::distance(
                                        NavGraph->GetNode(current.NodeId).GetPosition(),
                                        NavGraph->GetNode(neighborId).GetPosition()
                                        );

                const float h = glm::distance(NavGraph->GetNode(neighborId).GetPosition(), goalPos);
                const float f = g + h;

                if (!allNodes.count(neighborId) || g < allNodes[neighborId].CostSoFar)
                {
                    NodeRecord neighborRecord{neighborId, g, f, current.NodeId};
                    allNodes[neighborId] = neighborRecord;
                    openList.push(neighborRecord);
                }
            }
        }

        Path.clear();
    }

    std::vector<int> AStar::GetPath() const
    {
        return Path;
    }

    void AStar::SetGraph(const Engine::Graph* NewGraph)
    {
        NavGraph = NewGraph;
    }

    void AStar::SetStartPosition(const glm::vec3& Position)
    {
        int startNodeId = GetNodeIdFromPosition(Position);
        if (startNodeId == -1)
        {
            spdlog::warn("Can't find node for the starting position!");
            return;
        }
        StartId = startNodeId;
    }

    void AStar::SetGoalPosition(const glm::vec3& Position)
    {
        int goalNodeId = GetNodeIdFromPosition(Position);
        if (goalNodeId == -1)
        {
            spdlog::warn("Can't find node for the goal position!");
            return;
        }
        GoalId = goalNodeId;
    }

    void AStar::ComputePath(const glm::vec3& GoalPosition)
    {
        if (!NavGraph)
        {
            spdlog::warn("NavGraph is null!");
            return;
        }

        SetStartPosition(GetOwner()->GetTransform()->GetPosition());
        SetGoalPosition(GoalPosition);

        if (StartId < 0 || GoalId < 0)
        {
            spdlog::warn("Incorrect node IDs!");
            return;
        }

        ObjectPosition = glm::vec2(GetOwner()->GetTransform()->GetPosition().x,
                                   GetOwner()->GetTransform()->GetPosition().z);

        Path.clear();
        CurrentPathIndex = 0;

        FindPath(StartId, GoalId);

        if (Path.empty())
        {
            spdlog::warn("No available path!");
        }
    }

    void AStar::Update(const float DeltaTime)
    {
        if (CurrentPathIndex >= Path.size())
        {
            StartId = -1;
            GoalId = -1;
            Path.clear();
            CurrentPathIndex = 0;
        }

        if (!GetOwner() || Path.empty() || !NavGraph)
            return;


        const glm::vec2& targetPos = NavGraph->GetNode(Path[CurrentPathIndex]).GetPosition();
        glm::vec2 direction = targetPos - ObjectPosition;

        float distance = glm::length(direction);

        if (distance < 0.001f)
        {
            ObjectPosition = targetPos;
            CurrentPathIndex++;
            return;
        }

        direction = glm::normalize(direction);

        float targetAngle = glm::degrees(glm::atan(direction.x, direction.y));
        float currentAngle = GetOwner()->GetTransform()->GetEulerAngles().y;
        float angleDiff = glm::mod(targetAngle - currentAngle + 540.0f, 360.0f) - 180.0f;

        float rotationSpeed = 180.0f * DeltaTime;
        float newAngle = currentAngle + glm::clamp(angleDiff, -rotationSpeed, rotationSpeed);

        glm::vec3 rotation = GetOwner()->GetTransform()->GetEulerAngles();
        rotation.y = newAngle;
        GetOwner()->GetTransform()->SetEulerAngles(rotation);

        glm::vec2 forward = glm::normalize(
                glm::vec2(glm::sin(glm::radians(newAngle)), glm::cos(glm::radians(newAngle))));
        float dot = glm::dot(forward, direction);
        float angleBetween = glm::degrees(std::acos(glm::clamp(dot, -1.0f, 1.0f)));

        if (angleBetween < 70.0f)
        {
            float angleFactor = 1.0f - (angleBetween / 70.0f);
            float step = MoveSpeed * DeltaTime * angleFactor;

            if (step >= distance)
            {
                ObjectPosition = targetPos;
                CurrentPathIndex++;
            }
            else
            {
                ObjectPosition += direction * step;
            }
        }

        glm::vec3 currentPos3D = GetOwner()->GetTransform()->GetPosition();
        glm::vec3 newPos = glm::vec3(ObjectPosition.x, currentPos3D.y, ObjectPosition.y);
        GetOwner()->GetTransform()->SetPosition(newPos);
    }

    int AStar::GetNodeIdFromPosition(const glm::vec3& Position) const
    {
        if (!NavGraph)
            return -1;

        const auto& nodes = NavGraph->GetAllNodes();
        float closestDistance = FLT_MAX;
        int closestNodeId = -1;

        for (const auto& nodePair : nodes)
        {
            const glm::vec2& nodePosition = nodePair.second.GetPosition();

            float distance = glm::length(glm::vec2(Position.x, Position.z) - nodePosition);

            if (distance < closestDistance)
            {
                closestDistance = distance;
                closestNodeId = nodePair.first;
            }
        }

        if (closestDistance > 2 * NavMesh::Get().GetSpacing())
        {
            return -1;
        }

        return closestNodeId;
    }

    rapidjson::Value AStar::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(MoveSpeed)
        SERIALIZE_FIELD(NavMesh::Get().GetSpacing())
        END_COMPONENT_SERIALIZATION
    }

    void AStar::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(MoveSpeed)
        float spacing = NavMesh::Get().GetSpacing();
        DESERIALIZE_VALUE(spacing)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void AStar::DeserializeReferencesPass(const rapidjson::Value& Object,
                                          Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
}
