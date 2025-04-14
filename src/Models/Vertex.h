#pragma once

#include "glm/glm.hpp"

namespace Models
{
    /**
     * @brief Vertex Data.
     */
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec2 TexCoords;
        glm::vec3 Normal;
        glm::vec3 Tangent;

        Vertex(const glm::vec3& position, const glm::vec2& texCoords, const glm::vec3& normal,
               const glm::vec3& tangent);
    };
} // Models
