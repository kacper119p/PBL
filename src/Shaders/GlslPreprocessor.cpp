#include "GlslPreprocessor.h"

#include <sstream>
#include "ShaderException.h"
#include "Utility/FileUtilities.h"

namespace Shaders
{
    std::filesystem::path GLSLPreprocessor::IncludeDirectory("res/shaders/Include/");

    GLSLPreprocessor::~GLSLPreprocessor() = default;

    std::string GLSLPreprocessor::PreprocessFile(const std::string& Path)
    {
        std::unordered_set<std::string> includedFiles;
        return PreprocessInternal(Path, includedFiles);
    }

    std::string GLSLPreprocessor::PreprocessInternal(const std::string& Path,
                                                     std::unordered_set<std::string>& IncludedFiles)
    {
        std::string source;
        if (!Utility::GetTextFileContent(Path.c_str(), source))
        {
            throw ShaderException(std::format("Failed to read file '{}'", Path));
        }
        std::istringstream input(source);
        std::ostringstream output;
        std::string line;

        while (std::getline(input, line))
        {
            std::istringstream lineStream(line);
            std::string token;
            lineStream >> token;

            if (token == "#include")
            {
                std::string filename;
                lineStream >> filename;
                filename = filename.substr(1, filename.size() - 2); // Remove quote marks
                if (!IncludedFiles.contains(filename))
                {
                    std::string filePath = FindFile(filename, Path);
                    output << PreprocessInternal(filePath, IncludedFiles);
                    IncludedFiles.insert(filename);
                }
            }
            else
            {
                output << line << "\n";
            }
        }
        return output.str();
    }

    std::string GLSLPreprocessor::FindFile(const std::string& Path, const std::string& CallerPath)
    {
        //Search in Caller's directory.
        const std::filesystem::path callerPath(Path);
        std::filesystem::path CallerDirectory = callerPath.parent_path();
        std::filesystem::path path = callerPath / Path;

        if (exists(path))
        {
            std::filesystem::perms permissions = status(path).permissions();
            if ((permissions & std::filesystem::perms::owner_read) == std::filesystem::perms::owner_read)
            {
                return path.string();
            }
        }

        //Search in Include directory.
        path = IncludeDirectory / Path;

        if (exists(path))
        {
            std::filesystem::perms permissions = status(path).permissions();
            if ((permissions & std::filesystem::perms::owner_read) == std::filesystem::perms::owner_read)
            {
                return path.string();
            }
        }

        throw ShaderException(std::format("Could not include {} in {}.", Path, CallerPath));
    }
} // Shaders
