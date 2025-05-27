#include "LeafNodes.h"
#include <queue>
#include <random>

#include "AiManager.h"
#include "AStar.h"
#include "NavMesh.h"
#include "Engine/EngineObjects/Entity.h"
#include "spdlog/spdlog.h"

namespace Engine
{
    NodeStatus IsPlayerInRangeNode::Tick(float DeltaTime)
    {
        bool inRange = Ai->IsPlayerInRange();

        if (inRange)
        {
            if (!Ai->IsChasing)
            {
                Ai->AStarComponent->ClearPath(Ai->GetOwner());
            }
            Ai->AStarComponent->SetMovementEnabled(false);
            Ai->IsChasing = true;
            //spdlog::info("Slime w zasiegu gracza");
        }
        else
        {
            Ai->IsChasing = false;
            Ai->ChaseTimer = 0.0f;
            // spdlog::info("Slime nie w zasiegu gracza");
            Ai->AStarComponent->SetMovementEnabled(true);
        }
        return inRange ? NodeStatus::Success : NodeStatus::Failure;
    }

    NodeStatus IsChaseTimerOverNode::Tick(float DeltaTime)
    {
        bool isChase = Ai->IsChaseTimerOver();
        //if (isChase)
        //spdlog::info("Poscig sie skonczyl, zaraz sie zatrzymuje");
        //else
        //spdlog::info("Poscig trwa");
        return isChase ? NodeStatus::Success : NodeStatus::Failure;
    }

    NodeStatus UpdateChaseTimerNode::Tick(float DeltaTime)
    {
        spdlog::info("UpdateChaseTimerNode");

        if (Ai->RestTimer >= Ai->RestCooldown)
        {
            Ai->ChaseTimer = 0.0f;
            Ai->RestTimer = 0.0f;
            Ai->IsResting = false;
            Ai->IsChasing = true;
            Ai->SetRestFinished(true);
            spdlog::info("Odpoczynek zakonczony");
            return NodeStatus::Success;
        }

        Ai->ChaseTimer += DeltaTime;

        spdlog::info("Chase time: {:.2f}/{:.2f}", Ai->ChaseTimer, Ai->GetChaseCooldown());

        return NodeStatus::Success;
    }

    NodeStatus UpdateRestTimerNode::Tick(float DeltaTime)
    {
        if (Ai->IsRestFinished())
        {
            return NodeStatus::Success;
        }

        if (!Ai->IsResting)
        {
            Ai->RestTimer = 0.0f;
            Ai->IsResting = true;
            spdlog::info("Start odpoczynku");
        }

        Ai->RestTimer += DeltaTime;

        if (Ai->RestTimer >= Ai->GetRestCooldown())
        {
            Ai->RestTimer = 0.0f;
            Ai->SetIsChasingPlayer(false);
            Ai->IsResting = false;
            Ai->SetRestFinished(true);
            spdlog::info("Odpoczynek zakonczony");
            return NodeStatus::Success;
        }

        //spdlog::info("Odpoczywa... {:.2f}/{:.2f}", Ai->RestTimer, Ai->GetRestCooldown());
        return NodeStatus::Running;
    }

    NodeStatus EntityStopNode::Tick(float)
    {
        //spdlog::info("Slime zatrzymuje sie ze zmeczenia.");
        Ai->AStarComponent->ClearPath(Ai->GetOwner());
        Ai->ChaseTimer = 0.0f;
        return NodeStatus::Success;
    }

    NodeStatus RunFromPlayerNode::Tick(float)
    {
        //spdlog::info("RunFromPlayerNode");

        if (!Ai->IsChasing || Ai->IsResting)
            return NodeStatus::Failure;

        Ai->SetRestFinished(false);
        Ai->AStarComponent->SetMovementEnabled(true);

        auto* navMesh = &Engine::NavMesh::Get();
        auto* graph = navMesh->GetGraph();

        glm::vec3 slimePos = Ai->GetOwner()->GetTransform()->GetPosition();
        glm::vec3 playerPos = Ai->GetPlayer()->GetTransform()->GetPosition();
        float currentDist = glm::distance(slimePos, playerPos);

        int currentNodeId = navMesh->GetNodeIdFromPosition(slimePos);
        if (currentNodeId == -1)
        {
            //spdlog::warn("RunFromPlayerNode: brak aktualnego wêz³a.");
            return NodeStatus::Failure;
        }

        const auto& allNodes = graph->GetAllNodes();
        int bestNodeId = -1;
        float bestDist = currentDist;

        for (const auto& [id, node] : allNodes)
        {
            if (!graph->AreConnected(currentNodeId, id))
                continue;

            glm::vec3 nodePos = node.GetPosition();
            float distToPlayer = glm::distance(nodePos, playerPos);

            if (distToPlayer > bestDist)
            {
                bestDist = distToPlayer;
                bestNodeId = id;
            }
        }

        if (bestNodeId == -1)
        {
            //spdlog::warn("RunFromPlayerNode: brak lepszego wêz³a do ucieczki.");
            return NodeStatus::Failure;
        }

        bool shouldUpdate = Ai->AStarComponent->IsPathFinished();

        if (!shouldUpdate)
        {
            glm::vec3 currentGoal = Ai->AStarComponent->GetGoalPosition();
            float goalDistToPlayer = glm::distance(currentGoal, playerPos);

            if (bestDist > goalDistToPlayer + 0.5f)
                shouldUpdate = true;
        }

        if (shouldUpdate)
        {
            glm::vec3 targetPos = allNodes.at(bestNodeId).GetPosition();
            Ai->AStarComponent->SetGoalPosition(targetPos);
            Ai->AStarComponent->SetMoveSpeed(Ai->GetFastMovementSpeed());

            //spdlog::info("RunFromPlayerNode: ustawiono nowy cel ucieczki (wêze³ {})", bestNodeId);
        }

        return NodeStatus::Success;
    }

    bool IsPathSafe(const std::vector<int>& path, const auto& allNodes, const glm::vec3& playerPos,
                    float detectionRange)
    {
        if (path.empty())
            return false;

        for (size_t i = 0; i + 1 < path.size(); ++i)
        {
            glm::vec3 startPos = allNodes.at(path[i]).GetPosition();
            glm::vec3 endPos = allNodes.at(path[i + 1]).GetPosition();

            const int samples = 10;
            for (int s = 0; s <= samples; ++s)
            {
                float t = float(s) / samples;
                glm::vec3 interpPos = glm::mix(startPos, endPos, t);
                if (glm::distance(interpPos, playerPos) <= detectionRange + 0.1f)
                    return false;
            }
        }

        return true;
    }

    NodeStatus WalkSlowlyNode::Tick(float DeltaTime)
    {
        if (Ai->TargetTrash)
            return NodeStatus::Failure;

        if (!Ai->AStarComponent->IsPathFinished())
            return NodeStatus::Success;

        if (Ai->IsChasing || Ai->IsResting)
            return NodeStatus::Failure;

        Ai->SetRestFinished(false);

        auto* navMesh = &Engine::NavMesh::Get();
        auto* graph = navMesh->GetGraph();

        glm::vec3 currentPos = Ai->GetOwner()->GetTransform()->GetPosition();
        glm::vec3 playerPos = Ai->GetPlayer()->GetTransform()->GetPosition();
        float detectionRange = Ai->GetPlayerRange();

        int currentNodeId = navMesh->GetNodeIdFromPosition(currentPos);
        if (currentNodeId == -1)
        {
            spdlog::warn("WalkSlowly: brak aktualnego wêz³a.");
            return NodeStatus::Failure;
        }

        int minDistance = 1 / NavMesh::Get().GetSpacing();
        const auto& allNodes = graph->GetAllNodes();

        std::unordered_map<int, int> distances;
        std::queue<int> q;
        q.push(currentNodeId);
        distances[currentNodeId] = 0;

        std::vector<int> candidates;

        while (!q.empty())
        {
            int nodeId = q.front();
            q.pop();
            int dist = distances[nodeId];

            if (dist >= minDistance)
            {
                glm::vec3 nodePos = allNodes.at(nodeId).GetPosition();
                float distToPlayer = glm::distance(nodePos, playerPos);

                if (distToPlayer > detectionRange + 1.0f)
                {
                    candidates.push_back(nodeId);
                }
            }

            for (const auto& [neighborId, node] : allNodes)
            {
                if (graph->AreConnected(nodeId, neighborId) &&
                    distances.find(neighborId) == distances.end())
                {
                    distances[neighborId] = dist + 1;
                    q.push(neighborId);
                }
            }
        }

        if (!candidates.empty())
        {
            std::shuffle(candidates.begin(), candidates.end(), std::mt19937(std::random_device()()));

            for (int chosenId : candidates)
            {
                Ai->AStarComponent->FindPath(currentNodeId, chosenId);
                const std::vector<int>& path = Ai->AStarComponent->GetPath();

                bool pathSafe = IsPathSafe(path, allNodes, playerPos, detectionRange);

                if (pathSafe)
                {
                    glm::vec3 targetPos = allNodes.at(chosenId).GetPosition();

                    Ai->AStarComponent->SetMoveSpeed(Ai->GetSlowMovementSpeed());
                    Ai->AStarComponent->SetGoalPosition(targetPos);

                    //spdlog::info("WalkSlowly: ruszamy do bezpiecznego wêz³a {}, z dala od gracza", chosenId);
                    return NodeStatus::Success;
                }
            }
        }

        //spdlog::warn("WalkSlowly: nie znaleziono bezpiecznych wêz³ów do chodzenia.");
        return NodeStatus::Failure;
    }

    NodeStatus IsTrashInRangeNode::Tick(float)
    {
        bool inRange = Ai->IsTrashNearby();

        if (inRange)
        {
            //spdlog::info("Slime w zasiegu smiecia");
            if (!Ai->TargetTrash && !Ai->AStarComponent->IsPathFinished())
            {
                Ai->AStarComponent->ClearPath(Ai->GetOwner());
            }
        }
        else
        {
            //spdlog::info("Slime nie w zasiegu smiecia");
        }
        return inRange ? NodeStatus::Success : NodeStatus::Failure;
    }

    NodeStatus WalkToTrashNode::Tick(float DeltaTime)
    {
        if (!Ai || Ai->TrashEntities.empty())
            return NodeStatus::Failure;

        if (Ai->TargetTrash && !Ai->AStarComponent->IsPathFinished())
        {
            return NodeStatus::Success;
        }
        spdlog::info("im in");
        glm::vec3 slimePos = Ai->GetOwner()->GetTransform()->GetPosition();
        Entity* closestTrash = nullptr;
        float closestDist = std::numeric_limits<float>::max();

        for (Entity* trash : Ai->TrashEntities)
        {
            if (!trash)
                continue;

            glm::vec3 trashPos = trash->GetTransform()->GetPositionWorldSpace();
            glm::vec2 slimeXZ(slimePos.x, slimePos.z);
            glm::vec2 trashXZ(trashPos.x, trashPos.z);
            float dist = glm::distance(slimeXZ, trashXZ);

            if (dist < closestDist)
            {
                closestDist = dist;
                closestTrash = trash;
            }
        }

        if (!closestTrash)
            return NodeStatus::Failure;

        Ai->TargetTrash = closestTrash;

        glm::vec3 trashPos = closestTrash->GetTransform()->GetPositionWorldSpace();
        Ai->AStarComponent->SetMoveSpeed(Ai->GetSlowMovementSpeed());
        Ai->AStarComponent->SetGoalPosition(trashPos);

        return NodeStatus::Success;
    }

    NodeStatus AbsorbTrashNode::Tick(float DeltaTime)
    {
        spdlog::info("Slime wchlania smiecia");
        if (!Ai || Ai->TrashEntities.empty())
            return NodeStatus::Failure;

        glm::vec3 slimePos = Ai->GetOwner()->GetTransform()->GetPosition();
        Entity* nearest = nullptr;
        float minDist = std::numeric_limits<float>::max();

        for (Entity* trash : Ai->TrashEntities)
        {
            if (!trash)
                continue;

            glm::vec3 trashPos = trash->GetTransform()->GetPositionWorldSpace();
            glm::vec2 slimeXZ(slimePos.x, slimePos.z);
            glm::vec2 trashXZ(trashPos.x, trashPos.z);
            float dist = glm::distance(slimeXZ, trashXZ);

            if (dist <= 1.5f && dist < minDist)
            {
                minDist = dist;
                nearest = trash;
            }
        }

        if (!nearest)
        {
            AbsorbStartTime = -1.0f;
            return NodeStatus::Failure;
        }

        if (AbsorbStartTime < 0.0f)
        {
            AbsorbStartTime = 0.0f;
            spdlog::info("Slime rozpoczyna wchlanianie...");
            return NodeStatus::Running;
        }

        Ai->AStarComponent->SetMoveSpeed(Ai->GetSlowMovementSpeed() / 4);
        AbsorbStartTime += DeltaTime;

        if (AbsorbStartTime < 2.0f)
            return NodeStatus::Failure;

        // Reset
        Ai->AStarComponent->SetMoveSpeed(Ai->GetSlowMovementSpeed());
        AbsorbStartTime = -1.0f;

        nearest->GetTransform()->SetParent(Ai->GetOwner()->GetTransform());

        const auto& children = Ai->GetOwner()->GetTransform()->GetChildren();
        size_t index = children.size() - 1;

        float angle = glm::two_pi<float>() * (float(index) / std::max(size_t(1), children.size()));
        glm::vec3 offset = glm::vec3(std::cos(angle), 0.0f, std::sin(angle)) * 2.0f;

        nearest->GetTransform()->SetPositionLocalSpace(offset);
        
        auto it = std::find(Ai->TrashEntities.begin(), Ai->TrashEntities.end(), nearest);
        if (it != Ai->TrashEntities.end())
            Ai->TrashEntities.erase(it);

        Ai->TargetTrash = nullptr;

        return NodeStatus::Success;
    }

}
