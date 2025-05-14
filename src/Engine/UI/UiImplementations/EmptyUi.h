#pragma once
#include "Engine/UI/Ui.h"

namespace Engine::Ui
{
    class EmptyUi final : public Ui
    {
    public:
        EmptyUi() = default;

    public:
        void Update(float DeltaTime) override;

        SERIALIZATION_EXPORT_UI(EmptyUi)
    };
}
