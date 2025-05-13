#include "GameModeFactory.h"

#include "Engine/EngineObjects/GameMode/DefaultGameMode.h"

namespace Engine
{
    GameMode* GameModeFactory::CreateObject(const std::string& TypeName)
    {
        const auto iterator = GetInstance()->Builders.find(TypeName);
        if (iterator == GetInstance()->Builders.end())
        {
            return new DefaultGameMode();
        }
        return iterator->second->Build();
    }
}
