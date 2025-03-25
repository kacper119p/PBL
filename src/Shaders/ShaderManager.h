#pragma once
#include <string>
#include <unordered_map>

#include "ComputeShader.h"
#include "Shader.h"
#include "ShaderSourceFiles.h"

namespace Shaders
{

    class ShaderManager final
    {
    private:
        static std::unordered_map<std::string, unsigned int> ShaderStages;
        static std::unordered_map<ShaderSourceFiles, Shader> ShaderPrograms;
        static std::unordered_map<std::string, ComputeShader> ComputeShaderPrograms;

    private:
        ShaderManager() = default;

    private:
        ~ShaderManager() = default;

    public:
        static Shader GetShader(const ShaderSourceFiles& ShaderSourceFiles);

        static ComputeShader GetComputeShader(const char* ShaderSourceFile);

        static void FreeResources();

    private:
        static unsigned int CompileShaderStage(const char* FilePath, GLenum Type);

        static unsigned int GetShaderStage(const char* FilePath, GLenum Type);

        static Shader CreateShader(const ShaderSourceFiles& ShaderSource);\

        static ComputeShader CreateComputeShader(const char* sourceFile);
    };
} // Shaders
