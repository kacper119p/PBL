#pragma once

#include <type_traits>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

namespace Models
{
    /**
     * @brief Represents Axis-aligned Bounding Box.
     * @tparam T Representation of point.
     */
    template<typename T>
    struct AABBox
    {
    public:
        /**
         * @brief Corner with the lowest coordinates.
         */
        T min;
        /**
         * @brief Corner with the highest coordinates.
         */
        T max;

    public:
        /**
         * @brief Constructs a new AABox with default values.
         */
        AABBox()
        {
            if constexpr (std::is_arithmetic_v<T>)
            {
                min = 0;
                max = 0;
            }
        }

        /**
         * @brief Construct a new AABBox with provided coordinates.
         * @param Min Corner with the lowest coordinates.
         * @param Max Corner with the highest coordinates.
         */
        AABBox(const T Min, const T Max) :
            min(Min),
            max(Max)
        {
        }

        /**
         * @brief Returns 8 corner points of the AABBox in local space.
         * @return Corners in local space.
         */
        [[nodiscard]] std::vector<glm::vec3> GetCorners() const
        {
            return {
                    {min.x, min.y, min.z},
                    {max.x, min.y, min.z},
                    {min.x, max.y, min.z},
                    {max.x, max.y, min.z},
                    {min.x, min.y, max.z},
                    {max.x, min.y, max.z},
                    {min.x, max.y, max.z},
                    {max.x, max.y, max.z}
            };
        }

        /**
         * @brief Transforms AABBox to world space using a matrix.
         * @param ModelMatrix Transformation matrix.
         * @return Transformed AABBox.
         */
        AABBox ToWorldSpace(const glm::mat4& ModelMatrix) requires std::is_same_v<T, glm::vec3>
        {
            std::vector<glm::vec3> corners = GetCorners();

            glm::vec3 minWorld = glm::vec3(ModelMatrix * glm::vec4(corners[0], 1.0f));
            glm::vec3 maxWorld = minWorld;

            for (size_t i = 1; i < corners.size(); ++i)
            {
                glm::vec3 worldCorner = glm::vec3(ModelMatrix * glm::vec4(corners[i], 1.0f));
                minWorld = glm::min(minWorld, worldCorner);
                maxWorld = glm::max(maxWorld, worldCorner);
            }

            return AABBox(minWorld, maxWorld);
        }
    };

    typedef AABBox<float> AABBox1;
    typedef AABBox<glm::vec2> AABBox2;
    typedef AABBox<glm::vec3> AABBox3;
} // Models
