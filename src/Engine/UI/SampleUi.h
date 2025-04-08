#pragma once
#include "Ui.h"

namespace Engine::Ui
{
    class SampleUi final : public Ui
    {
    public:
        SampleUi();

    public:
        void Update(float DeltaTime) override;
    };

}
