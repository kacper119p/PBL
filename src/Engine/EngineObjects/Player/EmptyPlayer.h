#pragma once
#include "Player.h"

namespace Engine
{

    class EmptyPlayer final : public Player
    {
    public:
        EmptyPlayer() = default;

    public:
        void Update(float DeltaTime) override;

    private:
        SERIALIZATION_EXPORT_PLAYER(EmptyPlayer)
    };
} // Engine
