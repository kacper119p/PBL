#include <format>
#include "ComputeShader.h"
#include "ShaderException.h"
#include "glad/glad.h"

namespace
{
    constexpr int ErrorLogSize = 512;
}

namespace Shaders
{
    ComputeShader::ComputeShader(const char* SourceFile)
    {
        unsigned int ComputeShaderId;
        try
        {
            ComputeShaderId = CompileShader(SourceFile, GL_COMPUTE_SHADER);
        } catch (ShaderException& e)
        {
            throw e;
        }

        Id = glCreateProgram();
        glAttachShader(Id, ComputeShaderId);
        glLinkProgram(Id);

        glDetachShader(Id, ComputeShaderId);
        glDeleteShader(ComputeShaderId);

        int success;
        char errorInfo[ErrorLogSize];
        glGetProgramiv(Id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(Id, ErrorLogSize, nullptr, errorInfo);
            glDeleteProgram(Id);
            throw ShaderException(std::format("Failed to link shader: {}\n", errorInfo));
        }
    }
} // Shaders
