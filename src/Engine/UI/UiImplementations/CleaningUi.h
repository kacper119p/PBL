#pragma once
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

    public:
        CleaningUi();

    public:
        ~CleaningUi() override = default;

    public:
        void Update(float DeltaTime) override;

        SERIALIZATION_EXPORT_UI(CleaningUi);
    };

}
