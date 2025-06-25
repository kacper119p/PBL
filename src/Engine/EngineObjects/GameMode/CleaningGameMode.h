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

        std::string MainMenuPath = "./res/prefabs/MainMenu.prefab";
        //std::string Level1Path = "./res/prefabs/Level1.prefab";
        //std::string Level2Path = "./res/prefabs/Level2.prefab";
        //std::string Level3Path = "./res/prefabs/Level3.prefab";

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
