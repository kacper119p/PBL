#pragma once
#include <cstdint>

namespace Engine
{
    class Texture
    {
    private:
        uint32_t TextureId;

    public:
        Texture() :
            TextureId(0)
        {
        }

        explicit Texture(const uint32_t TextureId) :
            TextureId(TextureId)
        {
        }

    public:
        [[nodiscard]] uint32_t GetId() const
        {
            return TextureId;
        }

        bool operator==(const Texture Other) const
        {
            return TextureId == Other.TextureId;
        }

        bool operator!=(const Texture Other) const
        {
            return !(*this == Other);
        }
    };

} // Textures
