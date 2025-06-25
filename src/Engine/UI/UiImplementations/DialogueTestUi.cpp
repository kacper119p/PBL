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
        DialogueWidget->PopLine();
    }

    void DialogueTestUi::Update(const float DeltaTime)
    {
        Timer -= DeltaTime;
        if (Timer <= 0 && DialogueWidget->IsAnimationFinished())
        {
            DialogueWidget->PopLine();
            Timer = std::numeric_limits<float>::max();
        }

        DialogueWidget->Update(DeltaTime);
    }
}
