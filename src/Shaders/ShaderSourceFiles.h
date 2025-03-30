#pragma once
#include <string_view>

namespace Shaders
{
    /**
     * @brief Source files for a new shader.
     */
    struct ShaderSourceFiles
    {
        friend std::hash<Shaders::ShaderSourceFiles>;

    public:
        std::string VertexShader;
        std::string GeometryShader;
        std::string FragmentShader;

        ShaderSourceFiles(const std::string& VertexShader, const std::string& GeometryShader,
                          const std::string& FragmentShader) :
            VertexShader(VertexShader), GeometryShader(GeometryShader), FragmentShader(FragmentShader)
        {
        }

        ShaderSourceFiles(const char* VertexShader, const char* GeometryShader, const char* FragmentShader) :
            VertexShader(std::string(VertexShader)),
            GeometryShader(GeometryShader == nullptr ? std::string() : std::string(GeometryShader)),
            FragmentShader(std::string(FragmentShader))
        {
        }

        bool operator==(const ShaderSourceFiles& Other) const
        {
            return VertexShader == Other.VertexShader && GeometryShader == Other.GeometryShader && FragmentShader ==
                   Other.FragmentShader;
        }

        bool operator!=(const ShaderSourceFiles& Other) const
        {
            return !(*this == Other);
        }
    };

} // Shaders

template<>
struct std::hash<Shaders::ShaderSourceFiles>
{
    static size_t CombineHashes(const size_t A, const size_t B)
    {
        return A ^ (B + 0x9e3779b9 + (A << 6) + (A >> 2));
    }

    size_t operator()(const Shaders::ShaderSourceFiles& ShaderSourceFiles) const noexcept
    {
        constexpr std::hash<std::string> hasher;
        size_t hash = hasher(ShaderSourceFiles.VertexShader);
        hash = CombineHashes(hash, hasher(ShaderSourceFiles.GeometryShader));
        hash = CombineHashes(hash, hasher(ShaderSourceFiles.FragmentShader));
        return hash;
    }
};
