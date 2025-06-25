#pragma once
#include "Engine/UI/DialogueWidget.h"
#include "Engine/UI/Ui.h"

namespace Engine::Ui
{

    class DialogueTestUi final : public Ui
    {
    private:
        DialogueWidget* DialogueWidget;
        float Timer = 5.0f;

    public:
        DialogueTestUi();

    public:
        void Update(float DeltaTime) override;

        SERIALIZATION_EXPORT_UI(DialogueTestUi)
    };

}
