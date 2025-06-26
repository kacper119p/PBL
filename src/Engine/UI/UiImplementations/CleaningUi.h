#pragma once
#include <memory>
#include "Audio/AudioManager.h"
#include "Engine/UI/DialogueWidget.h"
#include "Engine/UI/Image.h"
#include "Engine/UI/Text.h"
#include "Engine/UI/Ui.h"

namespace Engine::Ui
{

    class CleaningUi final : public Ui
    {
    private:
        Image* TaskListBackground = nullptr;
        Image* SummaryBackground = nullptr;
        Image* Grade = nullptr;
        Image* Frame = nullptr;

        Text* TrashText = nullptr;
        Text* FloorText = nullptr;
        Text* WeaponText = nullptr;
        Text* BooksText = nullptr;
        Text* CoinsText = nullptr;
        Text* DontFurniture = nullptr;
        Text* DontWeapons = nullptr;
        Text* DontBooks = nullptr;
        Text* DontCoins = nullptr;
        Text* AdditionalTasks = nullptr;

        Text* TrashProgressText = nullptr;
        Text* FloorProgressText = nullptr;
        Text* WeaponProgressText = nullptr;
        Text* BooksProgressText = nullptr;
        Text* CoinsProgressText = nullptr;

        Text* DontFurnitureFailed = nullptr;
        Text* DontWeaponsFailed = nullptr;
        Text* DontBooksFailed = nullptr;
        Text* DontCoinsFailed = nullptr;

        Text* TimerText = nullptr;

        DialogueWidget* DialogueWidget = nullptr;

        bool ShowSummary = false;

        glm::vec4 PositiveColor = glm::vec4(0.0, 0.15, 0.0, 1);
        glm::vec4 FailedColor = glm::vec4(0.8, 0.05, 0.0, 1);
        glm::vec4 HeaderColor = glm::vec4(0.1, 0.0, 0.0, 1);

        float TaskListFontSize = 25.0f;

        float ReferenceBloodFill = 0.0f;
        int32_t ReferenceTrashCount = 0;

        bool HideKeyPressedLastFrame = false;
        bool Hidden = true;
        float HiddenTime = 1.0f;

        std::shared_ptr<ma_sound> ListSound = AudioManager::GetInstance().CreateSoundInstance("lista");
        std::shared_ptr<ma_sound> TaskSound = AudioManager::GetInstance().CreateSoundInstance("task");
        std::shared_ptr<ma_sound> LevelEnd = AudioManager::GetInstance().CreateSoundInstance("win1");
        bool TrashTaskCompleted = false;
        bool FloorTaskCompleted = false;
        bool WeaponTaskCompleted = false;
        bool BooksTaskCompleted = false;
        bool CoinsTaskCompleted = false;

        float SummaryAnimationTime = 0.0f;
        bool SummaryAppeared = false;

        bool GradeAnimationStarted = false;
        bool GradeAnimationFinished = false;
        float GradeAnimationTime = 0.0f;

        bool PicturesLoaded = false;

    public:
        CleaningUi();

    public:
        ~CleaningUi() override = default;

    public:
        void Update(float DeltaTime) override;
        void SetupDuringLevel();
        void SetupAfterLevel();

    private:
        void DialogueStart();

        void DialogueUpdate(float DeltaTime);

        SERIALIZATION_EXPORT_UI(CleaningUi);
    };

}
