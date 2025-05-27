#pragma once
#include "GameMode.h"
#include "Engine/Components/BloodSystem/BloodManager.h"

namespace Engine
{

    class CleaningGameMode final : public GameMode
    {
    private:
        BloodManager* BloodManager;

    public:
        CleaningGameMode();

    public:
        ~CleaningGameMode() override;

    public:
        void Start() override;

        void Update(float DeltaTime) override;

        SERIALIZATION_EXPORT_GAME_MODE(CleaningGameMode)
    };

}
