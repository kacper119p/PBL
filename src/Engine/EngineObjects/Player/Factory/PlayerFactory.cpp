#include "PlayerFactory.h"

#include "Engine/EngineObjects/Player/DefaultPlayer.h"

namespace Engine
{
    Player* PlayerFactory::CreateObject(const std::string& TypeName)
    {
        const auto iterator = GetInstance()->Builders.find(TypeName);
        if (iterator == GetInstance()->Builders.end())
        {
            return new DefaultPlayer();
        }
        return iterator->second->Build();
    }
} // Engine
