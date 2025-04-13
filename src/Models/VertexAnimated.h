#pragma once

#include "glm/glm.hpp"
#define MAX_BONE_INFLUENCE 4
namespace Models
{
    /**
     * @brief Vertex Data.
     */
    struct VertexAnimated
    {
        glm::vec3 Position;
        glm::vec2 TexCoords;
        glm::vec3 Normal;
        glm::vec3 Tangent;

        VertexAnimated(const glm::vec3& position, const glm::vec2& texCoords, const glm::vec3& normal,
               const glm::vec3& tangent);
        VertexAnimated() = default;

        int m_BoneIDs[MAX_BONE_INFLUENCE];
        // weights from each bone
        float m_Weights[MAX_BONE_INFLUENCE];
    };
} // namespace Models
