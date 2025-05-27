#include "AStar.h"
#include <queue>
#include "NavMesh.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/EngineObjects/RayCast.h"
#include "Engine/EngineObjects/UpdateManager.h"
#include "Models/Model.h"
#include "Serialization/SerializationUtility.h"
#include "spdlog/spdlog.h"

namespace Engine
{
    AStar::AStar()
    {
    }

    AStar::~AStar()
    {
    }

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

        const glm::vec2& goalPos = glm::vec2(NavGraph->GetNode(GoalId).GetPosition().x,
                                             NavGraph->GetNode(GoalId).GetPosition().z);
        const glm::vec2& startPos = glm::vec2(NavGraph->GetNode(StartId).GetPosition().x,
                                              NavGraph->GetNode(StartId).GetPosition().z);

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
                SmoothPath();
                return;
            }

            closedList[current.NodeId] = true;

            for (int neighborId : NavGraph->GetNode(current.NodeId).GetNeighbors())
            {
                if (closedList.count(neighborId))
                    continue;

                const float g = current.CostSoFar + glm::distance(
                                        glm::vec2(NavGraph->GetNode(current.NodeId).GetPosition().x,
                                                  NavGraph->GetNode(current.NodeId).GetPosition().z),
                                        glm::vec2(NavGraph->GetNode(neighborId).GetPosition().x,
                                                  NavGraph->GetNode(neighborId).GetPosition().z)
                                        );

                const float h = glm::distance(glm::vec2(NavGraph->GetNode(neighborId).GetPosition().x,
                                                        NavGraph->GetNode(neighborId).GetPosition().z), goalPos);
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
        int startNodeId = NavMesh::Get().GetNodeIdFromPosition(Position);
        if (startNodeId == -1)
        {
            spdlog::warn("Can't find node for the starting position!");
            return;
        }
        StartId = startNodeId;
        StartPosition = Position;
    }

    void AStar::SetGoalPosition(const glm::vec3& Position)
    {
        int goalNodeId = NavMesh::Get().GetNodeIdFromPosition(Position);
        if (goalNodeId == -1)
        {
            spdlog::warn("Can't find node for the goal position!");
            return;
        }
        GoalId = goalNodeId;
        GoalPosition = Position;
    }

    void AStar::ComputePath(const glm::vec3& GoalPosition, Entity* Entity)
    {
        if (!NavGraph)
        {
            spdlog::warn("NavGraph is null!");
            return;
        }

        glm::vec3 pos3D = Entity->GetTransform()->GetPosition();
        ObjectPosition = glm::vec2(pos3D.x, pos3D.z);

        SetStartPosition(pos3D);
        SetGoalPosition(GoalPosition);

        if (StartId < 0 || GoalId < 0)
        {
            spdlog::warn("Incorrect node IDs!");
            return;
        }

        Path.clear();
        CurrentPathIndex = 0;

        FindPath(StartId, GoalId);

        if (Path.empty())
        {
            spdlog::warn("No available path!");
        }
    }

    float AStar::GetMoveSpeed() const
    {
        return MoveSpeed;
    }

    void AStar::SetMoveSpeed(float Speed)
    {
        MoveSpeed = Speed;
    }

    glm::vec3 AStar::GetGoalPosition() const
    {
        return GoalPosition;
    }

    int AStar::GetCurrentPathIndex() const
    {
        return CurrentPathIndex;
    }

    void AStar::SetCurrentPathIndex(int Index)
    {
        if (Index >= 0 && Index < static_cast<int>(Path.size()))
            CurrentPathIndex = Index;
    }

    bool AStar::IsLineWalkable(int FromId, int ToId)
    {
        if (!NavGraph)
            return false;

        glm::vec3 start = NavGraph->GetNode(FromId).GetPosition();
        glm::vec3 end = NavGraph->GetNode(ToId).GetPosition();

        glm::vec3 rayDir = glm::vec3(0.0f, -1.0f, 0.0f);
        float segmentLength = glm::distance(start, end);
        int steps = static_cast<int>(segmentLength / 0.5f);
        if (steps <= 0)
            steps = 1;

        for (int i = 0; i <= steps; ++i)
        {
            float t = static_cast<float>(i) / static_cast<float>(steps);
            glm::vec3 samplePoint = glm::mix(start, end, t);
            samplePoint.y = 1000.0f;

            bool hitFound = false;

            for (const auto& modelPair : NavMesh::Get().GetModelTransforms())
            {
                Models::Model* model = modelPair.first;
                const glm::mat4& localToWorld = modelPair.second;

                for (int m = 0; m < model->GetMeshCount(); ++m)
                {
                    const auto& mesh = *model->GetMesh(m);
                    const auto& vertices = mesh.VerticesData;
                    const auto& indices = mesh.VertexIndices;

                    for (size_t j = 0; j + 2 < indices.size(); j += 3)
                    {
                        glm::vec3 v0 = glm::vec3(localToWorld * glm::vec4(vertices[indices[j]].Position, 1.0f));
                        glm::vec3 v1 = glm::vec3(localToWorld * glm::vec4(vertices[indices[j + 1]].Position, 1.0f));
                        glm::vec3 v2 = glm::vec3(localToWorld * glm::vec4(vertices[indices[j + 2]].Position, 1.0f));

                        if (RayCast::RayIntersectsTriangle(samplePoint, rayDir, v0, v1, v2))
                        {
                            hitFound = true;
                            break;
                        }
                    }
                    if (hitFound)
                        break;
                }
                if (hitFound)
                    break;
            }

            if (!hitFound)
            {
                return false;
            }
        }

        return true;
    }

    void AStar::SmoothPath()
    {
        if (!NavGraph || Path.size() < 3)
            return;

        std::vector<int> smoothed;
        size_t i = 0;

        while (i < Path.size())
        {
            smoothed.push_back(Path[i]);

            size_t j = Path.size() - 1;
            for (; j > i + 1; --j)
            {
                if (IsLineWalkable(Path[i], Path[j]))
                    break;
            }

            if (j == i)
                ++i;
            else
                i = j;
        }

        if (smoothed.back() != Path.back())
            smoothed.push_back(Path.back());

        Path = smoothed;
    }

    void AStar::UpdateMovement(const float DeltaTime, Entity* Entity)
    {
        if (!MovementEnabled)
            return;

        if (IsPathFinished())
        {
            ComputePath(GoalPosition, Entity);
        }

        if (CurrentPathIndex >= Path.size())
        {
            StartId = -1;
            GoalId = -1;
            Path.clear();
            CurrentPathIndex = 0;
        }

        if (!Entity || Path.empty() || !NavGraph)
            return;

        const glm::vec2& targetPos = glm::vec2(NavGraph->GetNode(Path[CurrentPathIndex]).GetPosition().x,
                                               NavGraph->GetNode(Path[CurrentPathIndex]).GetPosition().z);
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
        float currentAngle = Entity->GetTransform()->GetEulerAngles().y;
        float angleDiff = glm::mod(targetAngle - currentAngle + 540.0f, 360.0f) - 180.0f;

        float rotationSpeed = 180.0f * DeltaTime;
        float newAngle = currentAngle + glm::clamp(angleDiff, -rotationSpeed, rotationSpeed);

        glm::vec3 rotation = Entity->GetTransform()->GetEulerAngles();
        rotation.y = newAngle;
        Entity->GetTransform()->SetEulerAngles(rotation);

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

        glm::vec3 currentPos3D = Entity->GetTransform()->GetPosition();
        glm::vec3 newPos = glm::vec3(ObjectPosition.x, currentPos3D.y, ObjectPosition.y);
        Entity->GetTransform()->SetPosition(newPos);
    }

}
