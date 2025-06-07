#pragma once

#include "BehaviorTreeNode.h"
#include <vector>
#include <memory>

namespace Engine
{
    /**
     * @brief Executes child nodes in order until one fails or all succeed.
     */
    class Sequence : public BehaviorTreeNode
    {
    public:
        /**
         * @brief Adds a child node.
         * @param Node Child behavior node.
         */
        void AddChild(const std::shared_ptr<BehaviorTreeNode>& Node)
        {
            Children.push_back(Node);
        }

        /**
         * @brief Runs the sequence logic.
         * Returns Running, Failure, or Success based on child status.
         */
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
                else if (status == NodeStatus::Failure)
                {
                    Reset();
                    return NodeStatus::Failure;
                }
            }

            Reset();
            return NodeStatus::Success;
        }

        /**
         * @brief Resets this node and all children.
         */
        void Reset() override
        {
            CurrentIndex = 0;
            for (auto& child : Children)
                child->Reset();
        }

    private:
        std::vector<std::shared_ptr<BehaviorTreeNode>> Children; ///< Child nodes.
        size_t CurrentIndex = 0; ///< Index of the currently running child.
    };
}
