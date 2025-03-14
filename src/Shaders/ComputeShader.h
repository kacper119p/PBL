#pragma once

#include "ShaderBase.h"

namespace Shaders
{
    /**
     * @brief Compute shader.
     */
    class ComputeShader : public ShaderBase
    {
    public:
        explicit ComputeShader(const char* SourceFile);

    public:
        [[nodiscard]] glm::ivec3 GetWorkGroupSize() const
        {
            int localWorkGroupSize[3];
            glGetProgramiv(Id, GL_COMPUTE_WORK_GROUP_SIZE, localWorkGroupSize);
            return glm::ivec3(localWorkGroupSize[0], localWorkGroupSize[1], localWorkGroupSize[2]);
        }

        static void Dispatch(const glm::ivec3& WorkGroupCount)
        {
            glDispatchCompute(WorkGroupCount.x, WorkGroupCount.y, WorkGroupCount.z);
        }

        static void Dispatch()
        {
            glDispatchCompute(1, 1, 1);
        }
    };

} // Shaders
