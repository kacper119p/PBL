#pragma once
#include "Engine/UI/Ui.h"

namespace Engine::Ui
{
    class EmptyUi : public Ui
    {
    public:
        EmptyUi() = default;

    public:
        void Update(float DeltaTime) override;
    };
}
