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
