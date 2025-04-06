#pragma once

namespace Engine::Ui
{

    class UiElement
    {
    public:
        UiElement() = default;

    public:
        virtual ~UiElement() = default;

        virtual void Render() const = 0;
    };

}
