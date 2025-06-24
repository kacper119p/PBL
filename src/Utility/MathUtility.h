#pragma once
#include <glm/mat4x4.hpp>

namespace Utility
{
    /**
    * @brief Removes scaling from a 4x4 transformation matrix.
    * @param Matrix The input 4x4 transformation matrix.
    * @return A 4x4 matrix with scale removed, preserving rotation and translation.
    */
    glm::mat4 RemoveScaleMat4(const glm::mat4& Matrix);
} // Utility
