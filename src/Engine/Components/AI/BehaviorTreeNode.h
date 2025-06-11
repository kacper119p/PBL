#pragma once

namespace Engine
{
    enum class NodeStatus
    {
        Success,
        Failure,
        Running
    };

    class BehaviorTreeNode
    {
    public:
        virtual ~BehaviorTreeNode() = default;

        virtual NodeStatus Tick(float DeltaTime) = 0;

        virtual void Reset()
        {
        }
    };
}
