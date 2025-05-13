#pragma once
#include "GameMode.h"

namespace Engine
{

    class DefaultGameMode final : public GameMode
    {
        SERIALIZATION_EXPORT_GAME_MODE(DefaultGameMode)
    };

} // Engine
