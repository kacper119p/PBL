#include "VertexAnimated.h"

namespace Models
{
    VertexAnimated::VertexAnimated(const glm::vec3& position, const glm::vec2& texCoords, const glm::vec3& normal,
                   const glm::vec3& tangent) :
        Position(position), TexCoords(texCoords), Normal(normal), Tangent(tangent)
    {
    }
} // namespace Models
