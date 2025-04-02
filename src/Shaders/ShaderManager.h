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
        /**
         * @brief Gets cached shader or loads it from a file.
         * @param ShaderSourceFiles Shader's sourcefile.
         * @return Loaded shader.
         */
        static Shader GetShader(const ShaderSourceFiles& ShaderSourceFiles);

        /**
         * @brief Gets cached shader or loads it from a file.
         * @param ShaderSourceFile Shader's sourcefile.
         * @return Loaded shader.
         */
        static ComputeShader GetComputeShader(const char* ShaderSourceFile);

        /**
         * @brief Returns source files associated with a provided shader.
         * @param Shader Shader which source files should be found.
         * @return Shader's source files.
         */
        static ShaderSourceFiles GetShaderSourceFiles(Shader Shader);

        /**
         * @brief Returns source file associated with a provided shader.
         * @param Shader Shader which source file should be found.
         * @return Shader's source file.
         */
        static std::string GetShaderSourceFile(ComputeShader Shader);

        /**
         * @brief Deletes all shaders.
         */
        static void FreeResources();

    private:
        static unsigned int CompileShaderStage(const std::string& FilePath, GLenum Type);

        static unsigned int GetShaderStage(const std::string& FilePath, GLenum Type);

        static Shader CreateShader(const ShaderSourceFiles& ShaderSource);\

        static ComputeShader CreateComputeShader(const char* sourceFile);
    };
} // Shaders
