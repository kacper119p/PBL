#pragma once

#include "glm/glm.hpp"

namespace Engine
{

    /**
     * @brief Describes camera's properties to be used in shaders.
     */
    struct CameraRenderData
    {
    public:
        /**
         * @brief Camera's position.
         */
        glm::vec3 CameraPosition;
        /**
         * @brief Camera's transform matrix.
         */
        glm::mat4 ViewMatrix;
        /**
         * @brief Camera's projection Matrix.
         */
        glm::mat4 ProjectionMatrix;

    public:
        /**
         * @brief Constructs a new CameraRenderData and fills in all fields.
         * @param CameraPosition Camera's position.
         * @param ViewMatrix Camera's transform matrix.
         * @param ProjectionMatrix Camera's projection Matrix.
         */
        inline CameraRenderData(const glm::vec3& CameraPosition, const glm::mat4& ViewMatrix,
                                const glm::mat4& ProjectionMatrix) :
            CameraPosition(CameraPosition), ViewMatrix(ViewMatrix),
            ProjectionMatrix(ProjectionMatrix)
        {
        }
    };

} // Engine
