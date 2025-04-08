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
        T AddElement(UiElement* Parent);

    public:
        void Render() const;

        virtual void Update(float DeltaTime) = 0;
    };
}
