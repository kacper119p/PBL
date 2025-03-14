#pragma once

namespace Shaders
{
    /**
     * @brief Source files for a new shader.
     */
    struct ShaderSourceFiles
    {
        const char* VertexShader;
        const char* GeometryShader;
        const char* FragmentShader;

        inline ShaderSourceFiles(const char* VertexShader, const char* GeometryShader, const char* FragmentShader) :
            VertexShader(VertexShader), GeometryShader(GeometryShader), FragmentShader(FragmentShader)
        {
        }
    };

} // Shaders
