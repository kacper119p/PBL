#include "Frustum.h"

namespace Engine
{
    glm::vec4 Frustum::NormalizePlane(const glm::vec4& Plane)
    {
        float length = glm::length(glm::vec3(Plane));
        if (length > 0.0f)
            return Plane / length;
        return Plane;
    }

    void Frustum::UpdateFrustum(const CameraRenderData& CameraData)
    {
        const glm::mat4 vpMatrix = CameraData.ProjectionMatrix * CameraData.ViewMatrix;

        Planes[0] = glm::vec4( // Left
                vpMatrix[0][3] + vpMatrix[0][0],
                vpMatrix[1][3] + vpMatrix[1][0],
                vpMatrix[2][3] + vpMatrix[2][0],
                vpMatrix[3][3] + vpMatrix[3][0]);

        Planes[1] = glm::vec4( // Right
                vpMatrix[0][3] - vpMatrix[0][0],
                vpMatrix[1][3] - vpMatrix[1][0],
                vpMatrix[2][3] - vpMatrix[2][0],
                vpMatrix[3][3] - vpMatrix[3][0]);

        Planes[2] = glm::vec4( // Bottom
                vpMatrix[0][3] + vpMatrix[0][1],
                vpMatrix[1][3] + vpMatrix[1][1],
                vpMatrix[2][3] + vpMatrix[2][1],
                vpMatrix[3][3] + vpMatrix[3][1]);

        Planes[3] = glm::vec4( // Top
                vpMatrix[0][3] - vpMatrix[0][1],
                vpMatrix[1][3] - vpMatrix[1][1],
                vpMatrix[2][3] - vpMatrix[2][1],
                vpMatrix[3][3] - vpMatrix[3][1]);

        Planes[4] = glm::vec4( // Near
                vpMatrix[0][3] + vpMatrix[0][2],
                vpMatrix[1][3] + vpMatrix[1][2],
                vpMatrix[2][3] + vpMatrix[2][2],
                vpMatrix[3][3] + vpMatrix[3][2]);

        Planes[5] = glm::vec4( // Far
                vpMatrix[0][3] - vpMatrix[0][2],
                vpMatrix[1][3] - vpMatrix[1][2],
                vpMatrix[2][3] - vpMatrix[2][2],
                vpMatrix[3][3] - vpMatrix[3][2]);

        for (int i = 0; i < 6; ++i)
        {
            Planes[i] = NormalizePlane(Planes[i]);
        }
    }

    bool Frustum::IsBoxVisible(const Models::AABBox3& Box, const glm::mat4& ObjectToWorldMatrix) const
    {
        glm::vec3 corners[8];
        corners[0] = glm::vec3(ObjectToWorldMatrix * glm::vec4(Box.min.x, Box.min.y, Box.min.z, 1.0f));
        corners[1] = glm::vec3(ObjectToWorldMatrix * glm::vec4(Box.max.x, Box.min.y, Box.min.z, 1.0f));
        corners[2] = glm::vec3(ObjectToWorldMatrix * glm::vec4(Box.min.x, Box.max.y, Box.min.z, 1.0f));
        corners[3] = glm::vec3(ObjectToWorldMatrix * glm::vec4(Box.max.x, Box.max.y, Box.min.z, 1.0f));
        corners[4] = glm::vec3(ObjectToWorldMatrix * glm::vec4(Box.min.x, Box.min.y, Box.max.z, 1.0f));
        corners[5] = glm::vec3(ObjectToWorldMatrix * glm::vec4(Box.max.x, Box.min.y, Box.max.z, 1.0f));
        corners[6] = glm::vec3(ObjectToWorldMatrix * glm::vec4(Box.min.x, Box.max.y, Box.max.z, 1.0f));
        corners[7] = glm::vec3(ObjectToWorldMatrix * glm::vec4(Box.max.x, Box.max.y, Box.max.z, 1.0f));

        for (int i = 0; i < 6; ++i)
        {
            auto normal = glm::vec3(Planes[i]);
            float d = Planes[i].w;

            int insideCount = 0;

            for (const auto& corner : corners)
            {
                float distance = glm::dot(normal, corner) + d;
                if (distance >= 0.0f)
                {
                    insideCount++;
                    break;
                }
            }

            if (insideCount == 0)
            {
                return false;
            }
        }

        return true;
    }

}
