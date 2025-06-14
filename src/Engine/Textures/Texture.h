#pragma once
#include <cstdint>
#include "glad/glad.h"
#include "Utility/AssertionsUtility.h"

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

        [[nodiscard]] uint64_t GetHandleReadonly() const
        {
            if (TextureId == 0)
            {
                return 0;
            }
            const uint64_t handle = glGetTextureHandleARB(TextureId);
            return handle;
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
