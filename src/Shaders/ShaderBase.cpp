#include "ShaderBase.h"

#include "ShaderManager.h"

namespace Shaders
{
    void ShaderBase::Use() const
    {
        if (ShaderManager::BoundShaderId != Id)
        {
            glUseProgram(Id);
            ShaderManager::BoundShaderId = Id;
        }
    }
}
