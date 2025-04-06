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
        std::string Name;
        std::unordered_map<char, Glyph> Glyphs;
        uint32_t GlyphAtlas;

    public:
        Font() = default;

        explicit Font(const std::string& Name);

    public:
        [[nodiscard]] std::string GetName() const
        {
            return Name;
        }

        [[nodiscard]] const Glyph& GetGlyph(const char C) const
        {
            return Glyphs.at(C);
        }

        [[nodiscard]] const Glyph& operator[](const char C) const
        {
            return Glyphs.at(C);
        }

        [[nodiscard]] uint32_t GetGlyphAtlas() const
        {
            return GlyphAtlas;
        }

        bool operator==(const Font& Rhs) const
        {
            return Name == Rhs.Name;
        }

        bool operator!=(const Font& Rhs) const
        {
            return !(*this == Rhs);
        }
    };
}
