#pragma once
#include <cstdint>

namespace Engine
{
    class Texture
    {
    private:
        uint32_t TextureID;

    public:
        explicit Texture(const uint32_t TextureId) :
            TextureID(TextureId)
        {
        }

        [[nodiscard]] uint32_t GetId() const
        {
            return TextureID;
        }

        bool operator==(const Texture& other) const
        {
            return TextureID == other.TextureID;
        }
    };

} // Textures
