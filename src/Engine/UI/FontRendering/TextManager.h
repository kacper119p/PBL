#pragma once

#include <cstdint>
#include <string>

#include "Font.h"
#include "FontId.h"

namespace Engine::Ui
{
    class TextManager
    {
    private:
        static constexpr uint8_t MaxSupportedFonts = 8;
        static TextManager* Instance;

        Font Fonts[MaxSupportedFonts]{};
        uint8_t FontTopPointer = 0;

    private:
        TextManager();

    public:
        static void Initialize();

        static void Shutdown();

        static FontId GetFontId(const std::string& Name);

        static Font* GetFont(FontId Id);
    };

}
