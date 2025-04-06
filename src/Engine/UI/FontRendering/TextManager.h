#pragma once

#include <cstdint>

namespace Engine::Ui
{
    class TextManager
    {
    private:
        static constexpr uint8_t MaxSupportedFaces = 32;

        static TextManager* Instance;

        uint8_t FacesTopPointer = 0;

    private:
        TextManager();

    public:
        static void Initialize();

        static void Shutdown();


    };

}
