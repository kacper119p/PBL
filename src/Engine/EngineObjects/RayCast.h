#pragma once

#include "glm/glm.hpp"

namespace Engine
{
    class RayCast
    {
    public:
        /**
         * @brief Tests if a ray intersects a triangle.
         * @param RayOrigin The origin of the ray.
         * @param RayDirection The direction of the ray.
         * @param Vertex0, Vertex1, Vertex2 Triangle vertices.
         * @param HitPoint The intersection point, if any.
         * @return True if the ray intersects the triangle, otherwise false.
         */
        static bool RayIntersectsTriangle(const glm::vec3& RayOrigin, const glm::vec3& RayDirection,
                                          const glm::vec3& Vertex0, const glm::vec3& Vertex1, const glm::vec3& Vertex2,
                                          glm::vec3* HitPoint = nullptr)
        {
            const float epsilon = 1e-4f;

            glm::vec3 edge1 = Vertex1 - Vertex0;
            glm::vec3 edge2 = Vertex2 - Vertex0;

            glm::vec3 h = glm::cross(RayDirection, edge2);
            float determinant = glm::dot(edge1, h);

            if (determinant > -epsilon && determinant < epsilon)
                return false;

            float invDeterminant = 1.0f / determinant;
            glm::vec3 s = RayOrigin - Vertex0;
            float u = invDeterminant * glm::dot(s, h);

            if (u < 0.0f || u > 1.0f)
                return false;

            glm::vec3 q = glm::cross(s, edge1);
            float v = invDeterminant * glm::dot(RayDirection, q);

            if (v < 0.0f || u + v > 1.0f)
                return false;

            float t = invDeterminant * glm::dot(edge2, q);
            if (t > epsilon)
            {
                if (HitPoint)
                    *HitPoint = RayOrigin + RayDirection * t;
                return true;
            }

            return false;
        }
    };
}
