#include "Ui.h"

#include <ranges>

#include "Engine/EngineObjects/RenderingManager.h"
#include "Engine/EngineObjects/UpdateManager.h"

namespace Engine::Ui
{
    Ui::Ui()
    {
        RenderingManager::GetInstance()->RegisterUi(this);
        UpdateManager::GetInstance()->RegisterComponent(this);
    }

    Ui::~Ui()
    {
        RenderingManager::GetInstance()->UnregisterUi(this);
        UpdateManager::GetInstance()->UnregisterComponent(this);
    }

    void Ui::Render() const
    {
        for (UiElement* const uiElement : UiElements)
        {
            uiElement->Render();
        }
    }
}
