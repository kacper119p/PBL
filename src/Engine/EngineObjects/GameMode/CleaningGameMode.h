#pragma once
#include "GameMode.h"
#include "Engine/Components/BloodSystem/BloodManager.h"

namespace Engine
{

    class CleaningGameMode final : public GameMode
    {
    private:
        BloodManager* BloodManager;
        int ThrashCount = 0;
        float BloodFill = 0.0f;
        float cleanedPercent = 0.0f;
    public:
        CleaningGameMode();

    public:
        ~CleaningGameMode() override;

    public:
        void Start() override;

        void Update(float DeltaTime) override;

        bool isLevelFinished();

        SERIALIZATION_EXPORT_GAME_MODE(CleaningGameMode)
    };

}
