#pragma once
#include "UiElement.h"

namespace Engine::Ui
{

    class Ui
    {
    private:
        std::vector<UiElement*> UiElements;

    public:
        Ui();

    public:
        virtual ~Ui() = default;

    public:
        template<class T>
        T* AddElement(UiElement* Parent)
        {
            static_assert(std::is_base_of_v<UiElement, T>, "T must be derived from UiElement.");
            T* newElement = new T();
            if (Parent != nullptr)
            {
                newElement->GetRect().SetParent(&Parent->GetRect());
            }
            UiElements.emplace_back(newElement);
            return newElement;
        }

    public:
        void Render() const;

        virtual void Update(float DeltaTime) = 0;
    };
}
