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

    public:
        Shader() :
            ShaderBase(0)
        {
        }

    private:
        explicit Shader(const unsigned int ProgramId) :
            ShaderBase(ProgramId)
        {
        }

    public:
        bool operator==(const Shader Other) const
        {
            return Other.GetId() == this->GetId();
        }

        bool operator!=(const Shader Other) const
        {
            return !(*this == Other);
        }
    };
} //Shaders
