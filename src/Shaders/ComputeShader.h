#pragma once

#include "ShaderBase.h"

namespace Shaders
{
    /**
     * @brief Compute shader.
     */
    class ComputeShader final : public ShaderBase
    {
        friend class ShaderManager;

    private:
        explicit ComputeShader(const unsigned int ProgramID) :
            ShaderBase(ProgramID)
        {
        }

    public:
        [[nodiscard]] glm::ivec3 GetWorkGroupSize() const
        {
            int localWorkGroupSize[3];
            glGetProgramiv(GetId(), GL_COMPUTE_WORK_GROUP_SIZE, localWorkGroupSize);
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

    public:
        bool operator==(const ComputeShader Other) const
        {
            return Other.GetId() == this->GetId();
        }

        bool operator!=(const ComputeShader Other) const
        {
            return !(*this == Other);
        }
    };

} // Shaders
