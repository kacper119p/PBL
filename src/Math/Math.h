#pragma once

#include <intrin.h>

namespace Math
{
    inline float Min(float A, float B)
    {
        _mm_store_ss(&A, _mm_min_ss(_mm_set_ss(A), _mm_set_ss(B)));
        return A;
    }

    inline float Max(float A, float B)
    {
        _mm_store_ss(&A, _mm_max_ss(_mm_set_ss(A), _mm_set_ss(B)));
        return A;
    }


    inline auto Clamp(float X, float Min, float Max) -> float
    {
        _mm_store_ss(&X, _mm_min_ss(_mm_max_ss(_mm_set_ss(X), _mm_set_ss(Min)), _mm_set_ss(Max)));
        return X;
    }
}
