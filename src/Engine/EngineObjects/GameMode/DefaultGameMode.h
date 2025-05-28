#pragma once
#include "GameMode.h"

namespace Engine
{

    class DefaultGameMode final : public GameMode
    {
    public:
        void Start() override;

        void Update(float DeltaTime) override;

        SERIALIZATION_EXPORT_GAME_MODE(DefaultGameMode)
    };

}
