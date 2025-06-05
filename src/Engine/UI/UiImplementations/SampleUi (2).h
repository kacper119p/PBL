#pragma once

#include <cstdint>
#include "Engine/UI/Ui.h"

namespace Engine::Ui
{
    class Text;
}

namespace Engine::Ui
{
    class SampleUi2 final : public Ui
    {
    private:
        Text* ThrashCount;
        Text* VacuumCount;
        Text* VacuumVolume;
        float Timer = 0;
        uint8_t Change = 0;

    public:
        SampleUi2();

    public:
        void Update(float DeltaTime) override;

        SERIALIZATION_EXPORT_UI(SampleUi2);
    };

}
