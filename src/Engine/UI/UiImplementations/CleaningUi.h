#pragma once
#include <memory>
#include "Audio/AudioManager.h"
#include "Engine/UI/Image.h"
#include "Engine/UI/Text.h"
#include "Engine/UI/Ui.h"

namespace Engine::Ui
{

    class CleaningUi final : public Ui
    {
    private:
        Image* TaskListBackground;

        Text* TrashText;
        Text* FloorText;
        Text* WeaponText;
        Text* BooksText;
        Text* CoinsText;
        Text* DontFurniture;
        Text* DontWeapons;
        Text* DontBooks;
        Text* DontCoins;
        Text* AdditionalTasks;

        Text* TrashProgressText;
        Text* FloorProgressText;
        Text* WeaponProgressText;
        Text* BooksProgressText;
        Text* CoinsProgressText;

        Text* DontFurnitureFailed;
        Text* DontWeaponsFailed;
        Text* DontBooksFailed;
        Text* DontCoinsFailed;

        glm::vec4 PositiveColor = glm::vec4(0.0, 0.15, 0.0, 1);
        glm::vec4 FailedColor = glm::vec4(0.8, 0.05, 0.0,1);
        glm::vec4 HeaderColor = glm::vec4(0.1, 0.0, 0.0, 1);

        float TaskListFontSize = 25.0f;

        float ReferenceBloodFill = 0.0f;
        int32_t ReferenceTrashCount = 0;

        bool HideKeyPressedLastFrame = false;
        bool Hidden = true;
        float HiddenTime = 0.0f;

        std::shared_ptr<ma_sound> ListSound = AudioManager::GetInstance().CreateSoundInstance("lista");
        std::shared_ptr<ma_sound> TaskSound = AudioManager::GetInstance().CreateSoundInstance("task");
        bool TrashTaskCompleted = false;
        bool FloorTaskCompleted = false;

    public:
        CleaningUi();

    public:
        ~CleaningUi() override = default;

    public:
        void Update(float DeltaTime) override;

        SERIALIZATION_EXPORT_UI(CleaningUi);
    };

}
