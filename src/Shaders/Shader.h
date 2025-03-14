#pragma once

#include "ShaderSourceFiles.h"
#include "ShaderBase.h"

namespace Shaders
{
    /**
     * @brief Render pass shader.
     */
    class Shader : public ShaderBase
    {
    public:
        explicit Shader(const ShaderSourceFiles& MainPass);

    private:
        static unsigned int CompilePass(const ShaderSourceFiles& Pass);
    };
} //Shaders
