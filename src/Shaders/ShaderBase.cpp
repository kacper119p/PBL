#include <format>
#include "ShaderBase.h"

#include "GlslPreprocessor.h"
#include "ShaderException.h"

namespace
{
    constexpr int ErrorLogSize = 512;
}

namespace Shaders
{
    ShaderBase::~ShaderBase() = default;

    unsigned int Shaders::ShaderBase::CompileShader(const char* const FilePath, const GLenum Type)
    {
        const std::string preprocessedSource = GLSLPreprocessor::PreprocessFile(FilePath);
        const GLchar* const source = preprocessedSource.c_str();

        const unsigned int shaderId = glCreateShader(Type);
        glShaderSource(shaderId, 1, &source, nullptr);
        glCompileShader(shaderId);

        int success;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char errorInfo[ErrorLogSize];
            glGetShaderInfoLog(shaderId, ErrorLogSize, nullptr, errorInfo);
            throw ShaderException(std::format("Failed to compile shader: {}\n", errorInfo));
        }
        return shaderId;
    }
} // Shaders
