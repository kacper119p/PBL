#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace Shaders
{
    /**
     * @brief Base class for all shaders.
     */
    class ShaderBase
    {
        friend class ShaderManager;

    private:
        unsigned int Id;

    public:
        ShaderBase() = delete;

    protected:
        explicit ShaderBase(const unsigned int Id) :
            Id(Id)
        {
        }

    public:
        [[nodiscard]] bool IsNull() const
        {
            return Id == 0;
        }


        [[nodiscard]] unsigned int GetId() const
        {
            return Id;
        };

        void Use() const;

        static void SetUniform(const GLint UniformId, const GLfloat Value)
        {
            glUniform1f(UniformId, Value);
        }

        void SetUniform(const char* const Name, const GLfloat Value) const
        {
            SetUniform(glGetUniformLocation(Id, Name), Value);
        }


        static void SetUniform(const GLint UniformId, const glm::vec2& Value)
        {
            glUniform2f(UniformId, Value.x, Value.y);
        }

        void SetUniform(const char* const Name, const glm::vec2& Value) const
        {
            SetUniform(glGetUniformLocation(Id, Name), Value);
        }


        static void SetUniform(const GLint UniformId, const glm::vec3& Value)
        {
            glUniform3f(UniformId, Value.x, Value.y, Value.z);
        }

        void SetUniform(const char* const Name, const glm::vec3& Value) const
        {
            SetUniform(glGetUniformLocation(Id, Name), Value);
        }


        static void SetUniform(const GLint UniformId, const glm::vec4& Value)
        {
            glUniform4f(UniformId, Value.x, Value.y, Value.z, Value.w);
        }

        void SetUniform(const char* const Name, const glm::vec4& Value) const
        {
            SetUniform(glGetUniformLocation(Id, Name), Value);
        }


        static void SetUniform(const GLint UniformId, const GLint Value)
        {
            glUniform1i(UniformId, Value);
        }

        void SetUniform(const char* const Name, GLint Value) const
        {
            SetUniform(glGetUniformLocation(Id, Name), Value);
        }


        static void SetUniform(const GLint UniformId, const glm::ivec2 Value)
        {
            glUniform2i(UniformId, Value.x, Value.y);
        }

        void SetUniform(const char* const Name, const glm::ivec2 Value) const
        {
            SetUniform(glGetUniformLocation(Id, Name), Value);
        }


        static void SetUniform(const GLint UniformId, const glm::ivec3& Value)
        {
            glUniform3i(UniformId, Value.x, Value.y, Value.z);
        }

        void SetUniform(const char* const Name, const glm::ivec3& Value) const
        {
            SetUniform(glGetUniformLocation(Id, Name), Value);
        }


        static void SetUniform(const GLint UniformId, const glm::ivec4& Value)
        {
            glUniform4i(UniformId, Value.x, Value.y, Value.z, Value.w);
        }

        void SetUniform(const char* const Name, const glm::ivec4& Value) const
        {
            SetUniform(glGetUniformLocation(Id, Name), Value);
        }

        static void SetUniform(const GLint UniformId, const bool Value)
        {
            glUniform1i(UniformId, static_cast<int>(Value));
        }

        void SetUniform(const char* const Name, const bool Value) const
        {
            SetUniform(glGetUniformLocation(Id, Name), Value);
        }

        static void SetUniform(const GLint UniformId, const GLuint Value)
        {
            glUniform1ui(UniformId, Value);
        }

        void SetUniform(const char* const Name, const GLuint Value) const
        {
            SetUniform(glGetUniformLocation(Id, Name), Value);
        }


        static void SetUniform(const GLint UniformId, const glm::uvec2 Value)
        {
            glUniform2ui(UniformId, Value.x, Value.y);
        }

        void SetUniform(const char* const Name, glm::uvec2 Value) const
        {
            SetUniform(glGetUniformLocation(Id, Name), Value);
        }


        static void SetUniform(const GLint UniformId, const glm::uvec3& Value)
        {
            glUniform3ui(UniformId, Value.x, Value.y, Value.z);
        }

        void SetUniform(const char* const Name, const glm::uvec3& Value) const
        {
            SetUniform(glGetUniformLocation(Id, Name), Value);
        }


        static void SetUniform(const GLint UniformId, const glm::uvec4& Value)
        {
            glUniform4ui(UniformId, Value.x, Value.y, Value.z, Value.w);
        }

        void SetUniform(const char* const Name, const glm::uvec4& Value) const
        {
            SetUniform(glGetUniformLocation(Id, Name), Value);
        }


        static void SetUniform(const GLint UniformId, const glm::mat4& Value)
        {
            glUniformMatrix4fv(UniformId, 1, false, glm::value_ptr(Value));
        }

        void SetUniform(const char* const Name, const glm::mat4& Value) const
        {
            SetUniform(glGetUniformLocation(Id, Name), Value);
        }

        static void SetTexture(const GLint UniformId, const GLint Value)
        {
            glUniform1i(UniformId, Value);
        }

        void SetTexture(const char* const Name, const GLint Value) const
        {
            SetTexture(glGetUniformLocation(Id, Name), Value);
        }

        static void SetTextureHandle(const GLint UniformId, const GLuint64 Value)
        {
            glUniformHandleui64ARB(UniformId, Value);
        }

        void SetTextureHandle(const char* const Name, const GLuint64 Value) const
        {
            SetTextureHandle(glGetUniformLocation(Id, Name), Value);
        }

        static void SetUniformArray(const GLint UniformId, const glm::vec3* Array, const GLsizei Count)
        {
            glUniform3fv(UniformId, Count, glm::value_ptr(*Array));
        }

        void SetUniformArray(const char* const Name, const glm::vec3* Array, const GLsizei Count) const
        {
            glUniform3fv(glGetUniformLocation(Id, Name), Count, glm::value_ptr(*Array));
        }

        GLint GetUniformLocation(const char* const Name) const
        {
            return glGetUniformLocation(Id, Name);
        }

        void Delete()
        {
            glDeleteProgram(Id);
        }
    };
} // Shaders
