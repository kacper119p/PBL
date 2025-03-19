#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

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
    };

    typedef AABBox<float> AABBox1;
    typedef AABBox<glm::vec2> AABBox2;
    typedef AABBox<glm::vec3> AABBox3;
} // Models
