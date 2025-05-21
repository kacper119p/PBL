#pragma once

#include "Models/AabBox.h"
#include "Engine/EngineObjects/CameraRenderData.h"
#include <array>

namespace Engine
{
    /**
     * @brief Represents a camera frustum.
     */
    class Frustum
    {
    public:
        /**
         * @brief Updates frustum planes based on the given camera view-projection matrix.
         * @param RenderData Contains camera's projection and view matrices.
         */
        void UpdateFrustum(const CameraRenderData& CameraData);

        /**
         * @brief Checks if the given world-space AABB is inside or intersects the frustum.
         * @param Box The bounding box to test.
         * @param ObjectToWorldMatrix Transformation matrix from local to world space.
         * @return True if the box is at least partially inside the frustum.
         */
        bool IsBoxVisible(const Models::AABBox3& Box, const glm::mat4& ObjectToWorldMatrix) const;

        /**
         * @brief Checks if the given world-space sphere is inside or intersects the frustum.
         * @param Center The center of the sphere.
         * @param Radius The radius of the sphere.
         * @param ObjectToWorldMatrix Transformation matrix from local to world space.
         * @return True if the sphere is at least partially inside the frustum.
         */
        bool IsSphereVisible(const glm::vec3& Center, float Radius, const glm::mat4& ObjectToWorldMatrix) const;

    private:
        /**
         * @brief Six frustum planes in world space, in the order:
         * Left, Right, Bottom, Top, Near, Far.
         */
        std::array<glm::vec4, 6> Planes = {};

        /**
         * @brief Normalizes the plane equation so that the normal has unit length.
         * @param Plane The unnormalized plane (xyz = normal, w = distance).
         * @return The normalized plane.
         */
        static glm::vec4 NormalizePlane(const glm::vec4& Plane);
    };
}
