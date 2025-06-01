#pragma once
#include <cmath>
#include <numbers>
/*
* This file contains implementations of functions described by
* https://easings.net/en
*/

namespace Math
{
    namespace EasingConstants
    {
        constexpr float C1 = 1.70158f;
        constexpr float C2 = C1 * 1.525f;
        constexpr float C3 = C1 + 1.0f;
        constexpr float C4 = (2.0f * std::numbers::pi_v) / 3.0f;
        constexpr float C5 = (2.0f * std::numbers::pi_v) / 4.5f;
        constexpr float N1 = 7.5625f;
        constexpr float D1 = 2.75f;
    }

    /**
    * @brief Returns value of the easing function.
    * @param X Absolute progress of the animation in range [0,1].
    * @return Easing function value at X.
    */
    inline float EaseInSine(const float X)
    {
        return 1.0f - std::cosf((X * std::numbers::pi_v) / 2.0f);
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseOutSine(const float X)
    {
        return std::sinf((X * std::numbers::pi_v) / 2.0f);
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInOutSine(const float X)
    {
        return -(std::cosf(std::numbers::pi_v * X) - 1.0f) / 2.0f;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInQuad(const float X)
    {
        return X * X;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseOutQuad(const float X)
    {
        const float oneMinusX = 1.0f - X;
        return 1.0f - oneMinusX * oneMinusX;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInOutQuad(const float X)
    {
        if (X < 0.5f)
        {
            return 2.0f * X * X;
        }
        const float part = -2.0f * X + 2.0f;
        return 1.0f - part * part / 2.0f;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInCubic(const float X)
    {
        return X * X * X;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseOutCubic(const float X)
    {
        const float oneMinusX = 1.0f - X;
        return 1 - oneMinusX * oneMinusX * oneMinusX;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInOutCubic(const float X)
    {
        if (X < 0.5f)
        {
            return 4.0f * X * X * X;
        }
        const float part = -2.0f * X + 2.0f;
        return 1.0f - part * part * part / 2.0f;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInQuart(const float X)
    {
        const float x2 = X * X;
        return x2 * x2;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseOutQuart(const float X)
    {
        const float oneMinusX = 1.0f - X;
        const float oneMinusX2 = oneMinusX * oneMinusX;
        return oneMinusX2 * oneMinusX2;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInOutQuart(const float X)
    {
        if (X < 0.5f)
        {
            const float x2 = X * X;
            return 8.0f * x2 * x2;
        }
        const float part = -2.0f * X + 2.0f;
        const float part2 = part * part;
        return 1.0f - part2 * part2 / 2.0f;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInQuint(const float X)
    {
        const float x2 = X * X;
        return X * x2 * x2;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseOutQuint(const float X)
    {
        const float oneMinusX = 1.0f - X;
        const float oneMinusX2 = oneMinusX * oneMinusX;
        return oneMinusX * oneMinusX2 * oneMinusX2;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInOutQuint(const float X)
    {
        if (X < 0.5f)
        {
            const float x2 = X * X;
            return 16.0f * X * x2 * x2;
        }
        const float part = -2.0f * X + 2.0f;
        const float part2 = part * part;
        return 1.0f - part * part2 * part2 / 2.0f;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInExpo(const float X)
    {
        return X == 0.0f ? 0.0f : std::powf(2.0f, 10.0f * X - 10.0f);
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseOutExpo(const float X)
    {
        return X == 1.0f ? 1.0f : 1.0f - std::powf(2.0f, -10.0f * X);
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInOutExpo(const float X)
    {
        return X == 0.0f
                   ? 0.0f
                   : X == 1.0f
                   ? 1.0f
                   : X < 0.5f
                   ? std::powf(2.0f, 20.0f * X - 10.0f) / 2.0f
                   : (2.0f - std::powf(2, -20.0f * X + 10.0f)) / 2.0f;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInCirc(const float X)
    {
        return 1.0f - std::sqrtf(1.0f - std::powf(X, 2.0f));
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseOutCirc(const float X)
    {
        return std::sqrtf(1 - std::powf(X - 1.0f, 2.0f));
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInOutCirc(const float X)
    {
        return X < 0.5f
                   ? (1.0f - std::sqrtf(1 - std::powf(2.0f * X, 2.0f))) / 2.0f
                   : (std::sqrtf(1.0f - std::powf(-2.0f * X + 2.0f, 2.0f)) + 1.0f) / 2.0f;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInBack(const float X)
    {
        const float x2 = X * X;
        return EasingConstants::C3 * X * x2 - EasingConstants::C1 * x2;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseOutBack(const float X)
    {
        const float oneMinusX = 1.0f - X;
        const float oneMinusX2 = oneMinusX * oneMinusX;
        return 1.0f + EasingConstants::C3 * oneMinusX * oneMinusX2 + EasingConstants::C1 * oneMinusX2;
    }


    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInOutBack(const float X)
    {
        if (X < 0.5f)
        {
            const float part = 2.0f * X;
            const float part2 = part * part;
            return part2 * ((EasingConstants::C2 + 1) * part - EasingConstants::C2) / 2.0f;
        }
        const float part = -2.0f * X + 2.0f;
        const float part2 = part * part;

        return (part2 * ((EasingConstants::C2 + 1.0f) * (X * 2.0f - 2.0f) + EasingConstants::C2) + 2.0f) / 2.0f;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInElastic(const float X)
    {
        if (X == 0.0f)
        {
            return 0.0f;
        }
        if (X == 1.0f)
        {
            return 1.0f;
        }
        return -std::powf(2.0f, 10.0f * X - 10.0f) * std::sinf((X * 10.0f - 10.75f) * EasingConstants::C4);
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseOutElastic(const float X)
    {
        if (X == 0.0f)
        {
            return 0.0f;
        }
        if (X == 1.0f)
        {
            return 1.0f;
        }
        return std::powf(2.0f, -10.0f * X) * std::sinf((X * 10.0f - 0.75f) * EasingConstants::C4) + 1.0f;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInOutElastic(const float X)
    {
        if (X == 0.0f)
        {
            return 0.0f;
        }
        if (X == 1.0f)
        {
            return 1.0f;
        }
        if (X < 0.5f)
        {
            return -(std::powf(2.0f, 20.0f * X - 10.0f) * std::sinf((20.0f * X - 11.125f) * EasingConstants::C5)) /
                   2.0f;
        }
        return std::powf(2.0f, -20.0f * X + 10.0f) * std::sinf((20.0f * X - 11.125f) * EasingConstants::C5) / 2.0f +
               1.0f;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseOutBounce(float X)
    {
        if (X < 1.0f / EasingConstants::D1)
        {
            return EasingConstants::N1 * X * X;
        }
        if (X < 2.0f / EasingConstants::D1)
        {
            return EasingConstants::N1 * (X -= 1.5f / EasingConstants::D1) * X + 0.75f;
        }
        if (X < 2.5f / EasingConstants::D1)
        {
            return EasingConstants::N1 * (X -= 2.25f / EasingConstants::D1) * X + 0.9375f;
        }
        return EasingConstants::N1 * (X -= 2.625f / EasingConstants::D1) * X + 0.984375f;
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInBounce(const float X)
    {
        return 1.0f - EaseOutBounce(1.0f - X);
    }

    /**
     * @brief Returns value of the easing function.
     * @param X Absolute progress of the animation in range [0,1].
     * @return Easing function value at X.
     */
    inline float EaseInOutBounce(const float X)
    {
        return X < 0.5f
                   ? (1.0f - EaseOutBounce(1.0f - 2.0f * X)) / 2.0f
                   : (1.0f + EaseOutBounce(2.0f * X - 1.0f)) / 2.0f;
    }
}
