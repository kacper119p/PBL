#include "DialogueTestUi.h"

#include <limits>

namespace Engine::Ui
{

    DialogueTestUi::DialogueTestUi()
    {
        DialogueWidget = AddElement<class DialogueWidget>(nullptr);
        DialogueWidget->PushLine(
                "Lorem ipsum dolor sit amet, consectetur\nadipiscing elit, sed do eiusmod tempor.");
        DialogueWidget->PushLine(
                "Ut enim ad minim veniam, quis nostrud\nexercitation ullamco laboris nisi ut.");
    }

    void DialogueTestUi::Update(const float DeltaTime)
    {
        DialogueWidget->Show();
        DialogueWidget->PopLine();
        DialogueWidget->PopLine();
        DialogueWidget->Hide();

        DialogueWidget->Update(DeltaTime);
    }
}
