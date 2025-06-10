#include "LeafNodes.h"
#include <queue>
#include <random>
#include <tracy/Tracy.hpp>

#include "AiManager.h"
#include "AStar.h"
#include "NavMesh.h"
#include "Engine/Components/Game/Thrash.h"
#include "Engine/Components/Renderers/ModelRenderer.h"
#include "Engine/EngineObjects/Entity.h"
#include "spdlog/spdlog.h"

namespace Engine
{
    NodeStatus IsPlayerInRangeNode::Tick(float DeltaTime)
    {
        ZoneScoped;
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
        ZoneScoped;
        bool isChase = Ai->IsChaseTimerOver();
        //if (isChase)
        //spdlog::info("Poscig sie skonczyl, zaraz sie zatrzymuje");
        //else
        //spdlog::info("Poscig trwa");
        return isChase ? NodeStatus::Success : NodeStatus::Failure;
    }

    NodeStatus UpdateChaseTimerNode::Tick(float DeltaTime)
    {
        ZoneScoped;
        //spdlog::info("UpdateChaseTimerNode");
        if (Ai->RestTimer >= Ai->RestCooldown)
        {
            Ai->ChaseTimer = 0.0f;
            Ai->RestTimer = 0.0f;
            Ai->IsResting = false;
            Ai->IsChasing = true;
            Ai->SetRestFinished(true);
            //spdlog::info("Odpoczynek zakonczony");
            return NodeStatus::Success;
        }

        Ai->ChaseTimer += DeltaTime;
        //spdlog::info("Chase time: {:.2f}/{:.2f}", Ai->ChaseTimer, Ai->GetChaseCooldown());
        return NodeStatus::Success;
    }

    NodeStatus UpdateRestTimerNode::Tick(float DeltaTime)
    {
        ZoneScoped;
        if (Ai->IsRestFinished())
        {
            return NodeStatus::Success;
        }

        if (!Ai->IsResting)
        {
            Ai->RestTimer = 0.0f;
            Ai->IsResting = true;
            //spdlog::info("Start odpoczynku");
        }

        Ai->RestTimer += DeltaTime;

        if (Ai->RestTimer >= Ai->GetRestCooldown())
        {
            Ai->RestTimer = 0.0f;
            Ai->SetIsChasingPlayer(false);
            Ai->IsResting = false;
            Ai->SetRestFinished(true);
            //spdlog::info("Odpoczynek zakonczony");
            return NodeStatus::Success;
        }

        //spdlog::info("Odpoczywa... {:.2f}/{:.2f}", Ai->RestTimer, Ai->GetRestCooldown());
        return NodeStatus::Running;
    }

    NodeStatus EntityStopNode::Tick(float)
    {
        ZoneScoped;
        //spdlog::info("Slime zatrzymuje sie ze zmeczenia.");
        Ai->AStarComponent->ClearPath(Ai->GetOwner());
        Ai->ChaseTimer = 0.0f;
        return NodeStatus::Success;
    }

    NodeStatus RunFromPlayerNode::Tick(float)
    {
        ZoneScoped;
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
        ZoneScoped;
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
        ZoneScoped;
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

        float standThreshold = 0.05f;
        if (glm::distance(currentPos, LastPosition) < standThreshold)
        {
            TimeStandingStill += DeltaTime;
            if (TimeStandingStill >= MaxTimeStandingStill)
            {
                ShouldTurnAround = true;
                TimeStandingStill = 0.0f;
            }
        }
        else
        {
            TimeStandingStill = 0.0f;
            ShouldTurnAround = false;
        }

        LastPosition = currentPos;

        glm::vec3 playerPos = Ai->GetPlayer()->GetTransform()->GetPosition();
        float detectionRange = Ai->GetPlayerRange();

        glm::vec3 forward = Ai->GetOwner()->GetTransform()->GetForward();
        forward.y = 0.0f;
        forward = glm::normalize(forward);

        int currentNodeId = navMesh->GetNodeIdFromPosition(currentPos);
        if (currentNodeId == -1)
        {
            //spdlog::warn("WalkSlowly: brak aktualnego wêz³a.");
            return NodeStatus::Failure;
        }

        float maxMinSide = 0.0f;

        for (const auto& modelPair : NavMesh::Get().GetModelTransforms())
        {
            const auto* mesh = modelPair.first->GetMesh(0);
            if (!mesh)
                continue;

            const auto& aabb = mesh->GetAabBox();

            std::vector<glm::vec3> corners = aabb.GetCorners();

            glm::mat4 modelMatrix = modelPair.second;

            glm::vec3 globalMin(std::numeric_limits<float>::max());
            glm::vec3 globalMax(-std::numeric_limits<float>::max());

            for (const auto& corner : corners)
            {
                glm::vec4 transformed = modelMatrix * glm::vec4(corner, 1.0f);
                glm::vec3 pos = glm::vec3(transformed);

                globalMin = glm::min(globalMin, pos);
                globalMax = glm::max(globalMax, pos);
            }

            glm::vec3 globalSize = globalMax - globalMin;

            float minSide = std::min(globalSize.x, globalSize.z);
            if (minSide > maxMinSide)
                maxMinSide = minSide;
        }

        int minDistance = static_cast<int>(0.2f * maxMinSide);
        minDistance = std::max(minDistance, 1);

        const auto& allNodes = graph->GetAllNodes();

        std::unordered_map<int, int> distances;
        std::queue<int> q;
        q.push(currentNodeId);
        distances[currentNodeId] = 0;

        std::vector<int> candidates;

        const size_t maxCandidates = 100;
        while (!q.empty() && candidates.size() < maxCandidates)
        {
            ZoneScopedN("walkSlowly while");
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

            auto* node = graph->GetNode(nodeId);
            if (node)
            {
                for (int neighborId : node->GetNeighbors())
                {
                    if (distances.find(neighborId) == distances.end())
                    {
                        distances[neighborId] = dist + 1;
                        q.push(neighborId);
                    }
                }
            }
        }

        candidates.erase(std::remove_if(candidates.begin(), candidates.end(), [&](int nodeId)
        {
            auto* node = graph->GetNode(nodeId);
            if (!node)
                return true;

            if (node->GetNeighbors().size() < 8)
                return true;

            float distToCurrent = glm::distance(node->GetPosition(), currentPos);
            if (distToCurrent < minDistance)
                return true;

            if (std::find(LastChosenIds.begin(), LastChosenIds.end(), nodeId) != LastChosenIds.end())
                return true;

            glm::vec3 toCandidate = glm::normalize(node->GetPosition() - currentPos);
            toCandidate.y = 0.0f;

            float dot = glm::dot(forward, toCandidate);

            if (ShouldTurnAround)
            {
                if (dot > glm::cos(glm::radians(110.0f)))
                    return true;
            }
            else
            {
                if (dot < glm::cos(glm::radians(70.0f)))
                    return true;
            }

            return false;
        }), candidates.end());

        if (!candidates.empty())
        {
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(candidates.begin(), candidates.end(), g);

            for (int chosenId : candidates)
            {
                Ai->AStarComponent->FindPath(currentNodeId, chosenId);
                const std::vector<int>& path = Ai->AStarComponent->GetPath();

                if (IsPathSafe(path, allNodes, playerPos, detectionRange))
                {
                    glm::vec3 targetPos = allNodes.at(chosenId).GetPosition();

                    Ai->AStarComponent->SetMoveSpeed(Ai->GetSlowMovementSpeed());
                    Ai->AStarComponent->SetGoalPosition(targetPos);

                    LastChosenIds.push_back(chosenId);
                    if (LastChosenIds.size() > 10)
                        LastChosenIds.pop_front();

                    return NodeStatus::Success;
                }
            }
        }
        //spdlog::warn("WalkSlowly: nie znaleziono bezpiecznych wêz³ów do chodzenia.");
        return NodeStatus::Failure;
    }

    NodeStatus IsTrashInRangeNode::Tick(float)
    {
        ZoneScoped;
        Ai->RecalculateCurrentTrash();

        if (!Ai || Ai->CurrentTrashValue >= Ai->MaxTrashCapacity)
            return NodeStatus::Failure;

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
        ZoneScoped;
        if (!Ai || Ai->TrashEntities.empty())
            return NodeStatus::Failure;

        if (Ai->CurrentTrashValue >= Ai->MaxTrashCapacity)
            return NodeStatus::Failure;

        if (Ai->TargetTrash && !Ai->AStarComponent->IsPathFinished())
        {
            return NodeStatus::Success;
        }
        //spdlog::info("im in");
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
        ZoneScoped;
        if (!Ai || Ai->TrashEntities.empty())
            return NodeStatus::Failure;

        Entity* target = Ai->TargetTrash;

        glm::vec3 slimePos = Ai->GetOwner()->GetTransform()->GetPosition();
        glm::vec3 trashPos = target->GetTransform()->GetPositionWorldSpace();
        float distance = glm::distance(glm::vec2(slimePos.x, slimePos.z), glm::vec2(trashPos.x, trashPos.z));

        if (distance > 1.0f)
        {
            AbsorbStartTime = -1.0f;
            return NodeStatus::Failure;
        }

        if (!target)
        {
            AbsorbStartTime = -1.0f;
            return NodeStatus::Failure;
        }

        Thrash* trashComp = target->GetComponent<Thrash>();
        if (!trashComp)
        {
            AbsorbStartTime = -1.0f;
            Ai->TargetTrash = nullptr;
            return NodeStatus::Failure;
        }

        int value = static_cast<int>(trashComp->GetSize());

        if (AbsorbStartTime < 0.0f)
        {
            if (Ai->CurrentTrashValue + value > Ai->MaxTrashCapacity)
            {
                Ai->TargetTrash = nullptr;
                return NodeStatus::Failure;
            }

            AbsorbStartTime = 0.0f;
            return NodeStatus::Running;
        }

        if (!target)
        {
            AbsorbStartTime = -1.0f;
            Ai->TargetTrash = nullptr;
            return NodeStatus::Failure;
        }

        Ai->AStarComponent->SetMoveSpeed(Ai->GetSlowMovementSpeed() / 4);
        AbsorbStartTime += DeltaTime;

        if (AbsorbStartTime < 2.0f)
            return NodeStatus::Running;

        target->GetTransform()->SetParent(Ai->GetOwner()->GetTransform());

        Models::Model* model = Ai->GetOwner()->GetComponent<ModelRenderer>()->GetModel();
        if (!model || model->GetMeshCount() == 0)
        {
            AbsorbStartTime = -1.0f;
            Ai->TargetTrash = nullptr;
            return NodeStatus::Failure;
        }

        const auto& bounds = model->GetMesh(0)->GetAabBox();
        glm::vec3 min = bounds.min;
        glm::vec3 max = bounds.max;

        auto RandomInRange = [](float a, float b)
        {
            return a + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (b - a));
        };

        Models::Model* trashModel = target->GetComponent<ModelRenderer>()->GetModel();
        if (!trashModel || trashModel->GetMeshCount() == 0)
        {
            AbsorbStartTime = -1.0f;
            Ai->TargetTrash = nullptr;
            return NodeStatus::Failure;
        }

        const auto& trashBounds = trashModel->GetMesh(0)->GetAabBox();
        glm::vec3 trashSize = trashBounds.max - trashBounds.min;

        glm::vec3 availableMin = min + trashSize * 0.5f;
        glm::vec3 availableMax = max - trashSize * 0.5f;

        glm::vec3 offset = {
                RandomInRange(availableMin.x, availableMax.x),
                RandomInRange(availableMin.y, availableMax.y),
                RandomInRange(availableMin.z, availableMax.z)
        };

        target->GetTransform()->SetPositionLocalSpace(offset);

        auto it = std::find(Ai->TrashEntities.begin(), Ai->TrashEntities.end(), target);
        if (it != Ai->TrashEntities.end())
            Ai->TrashEntities.erase(it);

        Ai->TargetTrash = nullptr;
        Ai->AStarComponent->SetMoveSpeed(Ai->GetSlowMovementSpeed());
        AbsorbStartTime = -1.0f;

        return NodeStatus::Success;
    }

}
