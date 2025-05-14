#include "MathUtility.h"

namespace Utility
{
    glm::mat4 RemoveScaleMat4(const glm::mat4& Matrix)
    {
        glm::mat4 result;

        result[0] = glm::vec4(glm::normalize(glm::vec3(Matrix[0])), 0.0f);
        result[1] = glm::vec4(glm::normalize(glm::vec3(Matrix[1])), 0.0f);
        result[2] = glm::vec4(glm::normalize(glm::vec3(Matrix[2])), 0.0f);

        result[3] = Matrix[3];

        return result;
    }
} // Utility
