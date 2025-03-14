#pragma once
#include <filesystem>
#include <string>
#include <unordered_set>

namespace Shaders
{
    /**
     * @brief Simple preprocessor static-only class for resolving GLSL #includes.<br>
     * Rest of preprocessing directives is handled by driver.
     */
    class GLSLPreprocessor final
    {
    private:
        static std::filesystem::path IncludeDirectory;

    private:
        GLSLPreprocessor() = default;

    public:
        ~GLSLPreprocessor();

    public:
        /**
         * @brief Replaces #include directives recursively with included file's content.
         * @param Path Path to a file.
         * @return Preprocessed file's content.
         */
        static std::string PreprocessFile(const std::string& Path);

    private:
        static std::string PreprocessInternal(const std::string& Path, std::unordered_set<std::string>& IncludedFiles);

        static std::string FindFile(const std::string& Path, const std::string& CallerPath);
    };
} // Shaders
