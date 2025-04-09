#pragma once
#include <cstdint>

namespace Engine::Ui
{
    class Font;

    struct FontId
    {
    private:
        uint8_t Id;

    public:
        FontId() :
            Id(0)
        {
        }

        explicit FontId(const uint8_t Id) :
            Id(Id)
        {

        }

    public:
        [[nodiscard]] uint8_t GetId() const
        {
            return Id;
        }

        explicit operator uint8_t() const
        {
            return Id;
        }

        Font* operator*() const;

        const Font* operator->() const;

        bool operator==(const FontId& Rhs) const
        {
            return Id == Rhs.Id;
        }

        bool operator!=(const FontId& Rhs) const
        {
            return !(*this == Rhs);
        }
    };

}
