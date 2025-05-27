#pragma once

#include "BehaviorTreeNode.h"
#include <vector>
#include <memory>

namespace Engine
{
    class Selector : public BehaviorTreeNode
    {
    public:
        void AddChild(const std::shared_ptr<BehaviorTreeNode>& Node)
        {
            Children.push_back(Node);
        }

        NodeStatus Tick(float DeltaTime) override
        {
            for (size_t i = CurrentIndex; i < Children.size(); ++i)
            {
                NodeStatus status = Children[i]->Tick(DeltaTime);
                if (status == NodeStatus::Running)
                {
                    CurrentIndex = i;
                    return NodeStatus::Running;
                }
                else if (status == NodeStatus::Success)
                {
                    Reset();
                    return NodeStatus::Success;
                }
            }

            Reset();
            return NodeStatus::Failure;
        }

        void Reset() override
        {
            CurrentIndex = 0;
            for (auto& child : Children)
                child->Reset();
        }

    private:
        std::vector<std::shared_ptr<BehaviorTreeNode>> Children;
        size_t CurrentIndex = 0;
    };
}
