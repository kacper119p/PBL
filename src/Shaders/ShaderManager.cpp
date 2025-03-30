#include "ShaderManager.h"

#include <string>

#include "GlslPreprocessor.h"
#include "ShaderException.h"
#include "Utility/AssertionsUtility.h"

namespace
{
    constexpr int ErrorLogSize = 512;
}

namespace Shaders
{
    std::unordered_map<std::string, unsigned int> ShaderManager::ShaderStages;
    std::unordered_map<ShaderSourceFiles, Shader> ShaderManager::ShaderPrograms;
    std::unordered_map<std::string, ComputeShader> ShaderManager::ComputeShaderPrograms;

    Shader ShaderManager::GetShader(const ShaderSourceFiles& ShaderSourceFiles)
    {
        if (const auto iterator = ShaderPrograms.find(ShaderSourceFiles); iterator != ShaderPrograms.end())
        {
            return iterator->second;
        }

        Shader shader = CreateShader(ShaderSourceFiles);
        ShaderPrograms.emplace(ShaderSourceFiles, shader);
        return shader;
    }

    ComputeShader ShaderManager::GetComputeShader(const char* ShaderSourceFile)
    {
        std::string path(ShaderSourceFile);
        if (const auto iterator = ComputeShaderPrograms.find(path); iterator != ComputeShaderPrograms.end())
        {
            return iterator->second;
        }

        ComputeShader shader = CreateComputeShader(ShaderSourceFile);
        ComputeShaderPrograms.emplace(path, shader);
        return shader;
    }

    ShaderSourceFiles ShaderManager::GetShaderSourceFiles(const Shader Shader)
    {
        for (const auto& pair : ShaderPrograms)
        {
            if (pair.second == Shader)
            {
                return pair.first;
            }
        }
        CHECK_MESSAGE(false, "Shader is not loaded")
        return ShaderSourceFiles(nullptr, nullptr, nullptr);
    }

    std::string ShaderManager::GetShaderSourceFile(const ComputeShader Shader)
    {
        for (const auto& pair : ComputeShaderPrograms)
        {
            if (pair.second == Shader)
            {
                return pair.first;
            }
        }
        CHECK_MESSAGE(false, "Shader is not loaded")
        return std::string();
    }

    void ShaderManager::FreeResources()
    {
        for (const auto& pair : ShaderStages)
        {
            glDeleteShader(pair.second);
        }
        ShaderStages.clear();
        for (auto& pair : ShaderPrograms)
        {
            pair.second.Delete();
        }
        ShaderPrograms.clear();
        for (auto& pair : ComputeShaderPrograms)
        {
            pair.second.Delete();
        }
        ComputeShaderPrograms.clear();
    }

    unsigned int ShaderManager::CompileShaderStage(const char* const FilePath, const GLenum Type)
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
            glDeleteShader(shaderId);
            char errorInfo[ErrorLogSize];
            glGetShaderInfoLog(shaderId, ErrorLogSize, nullptr, errorInfo);
            throw ShaderException(std::format("Failed to compile shader: {}\n", errorInfo));
        }
        return shaderId;
    }

    unsigned int ShaderManager::GetShaderStage(const char* const FilePath, const GLenum Type)
    {
        const std::string filePath;
        if (const auto iterator = ShaderStages.find(filePath); iterator != ShaderStages.end())
        {
            return iterator->second;
        }

        unsigned int shaderId = CompileShaderStage(FilePath, Type);
        ShaderStages.emplace(filePath, shaderId);
        return shaderId;
    }

    Shader ShaderManager::CreateShader(const ShaderSourceFiles& ShaderSource)
    {
        unsigned int vertexShader;
        try
        {
            vertexShader = GetShaderStage(ShaderSource.VertexShader.c_str(), GL_VERTEX_SHADER);
        } catch (ShaderException&)
        {
            throw;
        }

        unsigned int geometryShader = 0;
        if (!ShaderSource.GeometryShader.empty())
        {
            try
            {
                geometryShader = GetShaderStage(ShaderSource.GeometryShader.c_str(), GL_GEOMETRY_SHADER);
            } catch (ShaderException&)
            {
                throw;
            }
        }

        unsigned int fragmentShader;
        try
        {
            fragmentShader = GetShaderStage(ShaderSource.FragmentShader.c_str(), GL_FRAGMENT_SHADER);
        } catch (ShaderException&)
        {
            throw;
        }

        const unsigned int id = glCreateProgram();

        glAttachShader(id, vertexShader);
        if (ShaderSource.GeometryShader.c_str())
        {
            glAttachShader(id, geometryShader);
        }
        glAttachShader(id, fragmentShader);
        glLinkProgram(id);

        int success;
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success)
        {
            char errorInfo[ErrorLogSize];
            glGetProgramInfoLog(id, ErrorLogSize, nullptr, errorInfo);
            glDeleteProgram(id);
            throw ShaderException(std::format("Failed to link shader: {}\n", errorInfo));
        }

        glDetachShader(id, vertexShader);
        if (!ShaderSource.GeometryShader.empty())
        {
            glDetachShader(id, geometryShader);
        }
        glDetachShader(id, fragmentShader);
        return Shader(id);
    }

    ComputeShader ShaderManager::CreateComputeShader(const char* const sourceFile)
    {
        unsigned int ComputeShaderId;
        try
        {
            ComputeShaderId = GetShaderStage(sourceFile, GL_COMPUTE_SHADER);
        } catch (ShaderException&)
        {
            throw;
        }

        const unsigned int programId = glCreateProgram();
        glAttachShader(programId, ComputeShaderId);
        glLinkProgram(programId);

        glDetachShader(programId, ComputeShaderId);

        int success;
        glGetProgramiv(programId, GL_LINK_STATUS, &success);
        if (!success)
        {
            char errorInfo[ErrorLogSize];
            glGetProgramInfoLog(programId, ErrorLogSize, nullptr, errorInfo);
            glDeleteProgram(programId);
            throw ShaderException(std::format("Failed to link shader: {}\n", errorInfo));
        }

        return ComputeShader(programId);
    }

} // namespace Shaders
