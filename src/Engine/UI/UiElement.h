#pragma once
#include "RectTransform.h"

namespace Engine::Ui
{

    class UiElement
    {
    public:
        RectTransform Rect;

    public:
        UiElement():
            Rect(this)
        {
        }

    public:
        virtual ~UiElement() = default;

    public:
        [[nodiscard]] RectTransform& GetRect()
        {
            return Rect;
        }

        [[nodiscard]] const RectTransform& GetRect() const
        {
            return Rect;
        }

        virtual void Render() = 0;
    };

}
