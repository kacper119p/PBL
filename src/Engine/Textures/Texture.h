#pragma once
#include <cstdint>

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
            CHECK_MESSAGE(TextureId != 0, "Texture not initialized");
            const uint64_t handle = glGetTextureHandleARB(TextureId);
            glMakeImageHandleResidentARB(handle, GL_READ_ONLY);
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
