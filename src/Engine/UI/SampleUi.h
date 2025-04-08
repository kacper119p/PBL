#pragma once
#include "Ui.h"

namespace Engine::Ui
{
    class Text;
    class Image;
}

namespace Engine::Ui
{
    class SampleUi final : public Ui
    {
    private:
        Image* MovingImage;
        Text* ChangingText;
        Text* ChangingText2;
        float Timer = 0;
        uint8_t Change = 0;

    public:
        SampleUi();

    public:
        void Update(float DeltaTime) override;
    };

}
