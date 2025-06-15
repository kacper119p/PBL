#pragma once
#include "Player.h"


namespace Engine
{
    class DefaultPlayer final : public Player
    {
        void Start() override;

        void Update() override;

        SERIALIZATION_EXPORT_PLAYER(DefaultPlayer)
    };
} // Engine
