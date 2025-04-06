#pragma once
#include <string>
#include <unordered_map>

#include "Glyph.h"

namespace Engine::Ui
{

    class Font
    {
    private:
        static std::string FontsFolder;

    private:
        std::unordered_map<char, Glyph> Glyphs;
        uint32_t GlyphAtlas;

    public:
        explicit Font(const std::string& Name);

    public:
        [[nodiscard]] const Glyph& GetGlyph(const char C) const
        {
            return Glyphs.at(C);
        }

        [[nodiscard]] const Glyph& operator[](const char C) const
        {
            return Glyphs.at(C);
        }
    };
}
