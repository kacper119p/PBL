#include "Ui.h"

#include <ranges>

namespace Engine::Ui
{
    Ui::Ui() = default;

    void Ui::Render() const
    {
        for (UiElement* const uiElement : UiElements)
        {
            uiElement->Render();
        }
    }
}
