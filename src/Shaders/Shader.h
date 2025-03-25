#pragma once

#include "ShaderBase.h"

namespace Shaders
{
    /**
     * @brief Render pass shader.
     */
    class Shader final : public ShaderBase
    {
        friend class ShaderManager;

    private:
        explicit Shader(const unsigned int ProgramId) :
            ShaderBase(ProgramId)
        {
        }
    };
} //Shaders
