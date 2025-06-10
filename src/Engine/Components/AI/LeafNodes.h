#pragma once

#include <queue>
#include "BehaviorTreeNode.h"
#include "AiManager.h"
#include "BehaviorTreeLeafNode.h"
#include "NavMesh.h"
#include "Engine/EngineObjects/Entity.h"
#include "glm/glm.hpp"

namespace Engine
{
    class IsPlayerInRangeNode : public BehaviorTreeLeafNode
    {
    public:
        IsPlayerInRangeNode(AiManager* Ai) :
            BehaviorTreeLeafNode(Ai)
        {
        }

        NodeStatus Tick(float DeltaTime) override;

    private:
        float DetectionRange = 5.0f;
    };

    class IsChaseTimerOverNode : public BehaviorTreeLeafNode
    {
    public:
        IsChaseTimerOverNode(AiManager* Ai) :
            BehaviorTreeLeafNode(Ai)
        {
        }

        NodeStatus Tick(float DeltaTime) override;

    };

    class EntityStopNode : public BehaviorTreeLeafNode
    {
    public:
        EntityStopNode(AiManager* Ai) :
            BehaviorTreeLeafNode(Ai)
        {
        }

        NodeStatus Tick(float DeltaTime) override;

    private:
        float StopTime = 2.0f;
        float Timer = 0.0f;
        bool Stopping = false;
    };

    class RunFromPlayerNode : public BehaviorTreeLeafNode
    {
    public:
        RunFromPlayerNode(AiManager* Ai) :
            BehaviorTreeLeafNode(Ai)
        {
        }

        NodeStatus Tick(float DeltaTime) override;

    private:
        float EscapeDistance = 8.0f;
    };

    class WalkSlowlyNode : public BehaviorTreeLeafNode
    {
    public:
        WalkSlowlyNode(AiManager* Ai) :
            BehaviorTreeLeafNode(Ai)
        {
        }

        NodeStatus Tick(float DeltaTime) override;

    private:
        std::deque<int> LastChosenIds;
        glm::vec3 LastPosition = glm::vec3(0.0f);
        float TimeStandingStill = 0.0f;
        const float MaxTimeStandingStill = 1.0f;
        bool ShouldTurnAround = false;

    };

    class IsTrashInRangeNode : public BehaviorTreeLeafNode
    {
    public:
        IsTrashInRangeNode(AiManager* Ai) :
            BehaviorTreeLeafNode(Ai)
        {
        }

        NodeStatus Tick(float DeltaTime) override;

    private:
        float TrashRange = 3.0f;
    };

    class WalkToTrashNode : public BehaviorTreeLeafNode
    {
    public:
        WalkToTrashNode(AiManager* Ai) :
            BehaviorTreeLeafNode(Ai)
        {
        }

        NodeStatus Tick(float DeltaTime) override;
    };

    class AbsorbTrashNode : public BehaviorTreeLeafNode
    {
    public:
        AbsorbTrashNode(AiManager* Ai) :
            BehaviorTreeLeafNode(Ai)
        {
        }

        NodeStatus Tick(float DeltaTime) override;

    private:
        float AbsorbStartTime = -1.0f;
    };

    class UpdateChaseTimerNode : public BehaviorTreeLeafNode
    {
    public:
        UpdateChaseTimerNode(AiManager* Ai) :
            BehaviorTreeLeafNode(Ai)
        {
        }

        NodeStatus Tick(float DeltaTime) override;

    };

    class UpdateRestTimerNode : public BehaviorTreeLeafNode
    {
    public:
        UpdateRestTimerNode(AiManager* Ai) :
            BehaviorTreeLeafNode(Ai)
        {
        }

        NodeStatus Tick(float DeltaTime) override;

    };
}
