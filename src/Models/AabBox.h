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
        [[nodiscard]] std::vector<glm::vec3> GetCorners() const requires std::is_same_v<T, glm::vec3>
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
            glm::vec3 minWorld = glm::vec3(min.x, min.y, min.z);
            glm::vec3 maxWorld = minWorld;

            glm::vec3 corners[7];
            corners[0] = glm::vec3(max.x, min.y, min.z);
            corners[1] = glm::vec3(min.x, max.y, min.z);
            corners[2] = glm::vec3(max.x, max.y, min.z);
            corners[3] = glm::vec3(min.x, min.y, max.z);
            corners[4] = glm::vec3(max.x, min.y, max.z);
            corners[5] = glm::vec3(min.x, max.y, max.z);
            corners[6] = glm::vec3(max.x, max.y, max.z);

            for (size_t i = 0; i < 7; ++i)
            {
                glm::vec3 worldPosition = glm::vec3(ModelMatrix * glm::vec4(corners[i], 1.0f));

                minWorld = glm::min(minWorld, worldPosition);
                maxWorld = glm::max(maxWorld, worldPosition);
            }

            return AABBox(minWorld, maxWorld);
        }
    };

    typedef AABBox<float> AABBox1;
    typedef AABBox<glm::vec2> AABBox2;
    typedef AABBox<glm::vec3> AABBox3;
} // Models
