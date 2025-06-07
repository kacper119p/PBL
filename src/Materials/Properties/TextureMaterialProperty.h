#pragma once
#include <cstdint>

#include "Engine/Textures/Texture.h"
#include "Shaders/Shader.h"

namespace Materials
{

    class TextureMaterialProperty
    {
    private:
        int32_t Location;
        uint32_t TextureId;
        uint64_t Handle;

    public:
        TextureMaterialProperty(const char* const Name, const Shaders::Shader& Owner) :
            Location(glGetUniformLocation(Owner.GetId(), Name)), TextureId(0), Handle(0)
        {
        }

        TextureMaterialProperty(const char* const Name, const Shaders::Shader& Owner, const Engine::Texture Value) :
            Location(glGetUniformLocation(Owner.GetId(), Name)), TextureId(Value.GetId()),
            Handle(Value.GetHandleReadonly())
        {
        }

        TextureMaterialProperty() :
            Location(-1), TextureId(0), Handle(0)
        {
        }

    public:
        ~TextureMaterialProperty() = default;

    public:
        /**
         * @brief Returns value stored by this property.
         */
        [[nodiscard]] Engine::Texture GetValue() const
        {
            return Engine::Texture(TextureId);
        }

        /**
         * @brief Sets value of this property.
         * @param Value A new value.
         */
        void SetValue(const Engine::Texture Value)
        {
            TextureId = Value.GetId();
            Handle = Value.GetHandleReadonly();
        }

        /**
         * @brief Returns cached location of uniform bound to this property.
         */
        [[nodiscard]] GLint GetLocation() const
        {
            return Location;
        }

        /**
         * @brief Sets uniform represented by this property.
         */
        void Bind() const
        {
            if (Handle != 0)
            {
                Shaders::ShaderBase::SetTextureHandle(Location, Handle);
            }
        }
    };

} // Materials
