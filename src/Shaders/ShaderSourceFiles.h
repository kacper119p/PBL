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
        const char* VertexShader;
        const char* GeometryShader;
        const char* FragmentShader;

        ShaderSourceFiles(const char* VertexShader, const char* GeometryShader, const char* FragmentShader) :
            VertexShader(VertexShader), GeometryShader(GeometryShader), FragmentShader(FragmentShader)
        {
        }

        bool operator==(const ShaderSourceFiles&) const
        {

            if (std::strcmp(VertexShader, VertexShader) != 0)
            {
                return false;
            }
            if (GeometryShader && std::strcmp(GeometryShader, GeometryShader) != 0)
            {
                return false;
            }
            if (std::strcmp(FragmentShader, FragmentShader) != 0)
            {
                return false;
            }
            return true;
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
        constexpr std::hash<std::string_view> hasher;
        size_t hash = hasher(ShaderSourceFiles.VertexShader);
        if (ShaderSourceFiles.GeometryShader)
        {
            hash = CombineHashes(hash, hasher(ShaderSourceFiles.GeometryShader));
        }
        hash = CombineHashes(hash, hasher(ShaderSourceFiles.FragmentShader));
        return hash;
    }
};
