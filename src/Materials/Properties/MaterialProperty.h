#pragma once
#include <string>

#include <glm/glm.hpp>
#include "Shaders/ShaderBase.h"

namespace Materials
{
    /**
     * @brief Represents shader uniform.
     * @tparam T Property type.
     */
    template<typename T>
    class MaterialProperty
    {
    private:
        GLint Location;
        T Value;

    public:
        MaterialProperty(const char* const Name, const Shaders::Shader& Owner) :
            Location(glGetUniformLocation(Owner.GetId(), Name))
        {
            if constexpr (std::is_arithmetic_v<T>)
            {
                Value = 0;
            }
            else
            {
                Value = T();
            }
        }

        MaterialProperty(const char* const Name, const Shaders::Shader& Owner, const T& value) :
            Location(glGetUniformLocation(Owner.GetId(), Name)), Value(value)
        {
        }

    public:
        ~MaterialProperty() = default;

    public:
        /**
         * @brief Returns value stored by this property.
         */
        [[nodiscard]] T GetValue() const
        {
            return Value;
        }

        /**
         * @brief Sets value of this property.
         * @param value A new value.
         */
        void SetValue(const T& value)
        {
            Value = value;
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
            Shaders::ShaderBase::SetUniform(Location, Value);
        }
    };

    typedef MaterialProperty<float> FloatMaterialProperty;
    typedef MaterialProperty<glm::vec2> Vector2MaterialProperty;
    typedef MaterialProperty<glm::vec3> Vector3MaterialProperty;
    typedef MaterialProperty<glm::vec4> Vector4MaterialProperty;
    typedef MaterialProperty<int> IntMaterialProperty;
    typedef MaterialProperty<glm::ivec2> IntVector2MaterialProperty;
    typedef MaterialProperty<glm::ivec3> IntVector3MaterialProperty;
    typedef MaterialProperty<glm::ivec4> IntVector4MaterialProperty;
    typedef MaterialProperty<unsigned int> UIntMaterialProperty;
    typedef MaterialProperty<glm::uvec2> UIntVector2MaterialProperty;
    typedef MaterialProperty<glm::uvec3> UIntVector3MaterialProperty;
    typedef MaterialProperty<glm::uvec4> UIntVector4MaterialProperty;
    typedef MaterialProperty<glm::mat4> Matrix4MaterialProperty;
    typedef MaterialProperty<int> TextureMaterialProperty;
} // Models
