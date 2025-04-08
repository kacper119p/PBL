#include "Ui.h"

#include <ranges>

namespace Engine::Ui
{
    Ui::Ui() = default;

    template<class T>
    T Ui::AddElement(UiElement* Parent)
    {
        static_assert(std::is_base_of_v<UiElement, T>, "T must be derived from UiElement.");
        T* newElement = new T();
        newElement->Parent = Parent;
        UiElements.emplace_back(newElement);
        return newElement;
    }

    void Ui::Render() const
    {
        for (const UiElement* const uiElement : UiElements)
        {
            uiElement->Render();
        }
    }
}
