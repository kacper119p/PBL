#pragma once
#include "Player.h"


namespace Engine
{
    class DefaultPlayer final : public Player
    {
        SERIALIZATION_EXPORT_PLAYER(DefaultPlayer)
    };
} // Engine
