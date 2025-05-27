#pragma once

#include "BehaviorTreeNode.h"

namespace Engine
{
    class AiManager;

    class BehaviorTreeLeafNode : public BehaviorTreeNode
    {
    public:
        explicit BehaviorTreeLeafNode(AiManager* Owner) :
            Ai(Owner)
        {
        }

        ~BehaviorTreeLeafNode() override = default;

    protected:
        AiManager* Ai;
    };
}
