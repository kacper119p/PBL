#pragma once
#include "RectTransform.h"

namespace Engine::Ui
{

    class UiElement
    {
    public:
        RectTransform Rect;

        UiElement() = default;

    public:
        UiElement(const UiElement&) :
            Rect(this)
        {
        }

    public:
        virtual ~UiElement() = default;

    public:
        virtual void Render() const = 0;
    };

}
