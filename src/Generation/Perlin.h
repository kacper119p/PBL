#pragma once

#include <vector>
#include <random>

/**
 * @brief Class for generating 2D Perlin noise.
 */
class Perlin
{
public:
    /**
     * @brief Constructs the Perlin noise generator with a given seed.
     * @param Seed Seed value for generating the permutation table.
     */
    explicit Perlin(unsigned int Seed = std::default_random_engine::default_seed);

    /**
     * @brief Generates 2D Perlin noise for given coordinates.
     * @param X X-coordinate in noise space.
     * @param Y Y-coordinate in noise space.
     * @return Noise value in the range [-1, 1].
     */
    double Noise(double X, double Y) const;

private:
    std::vector<int> P; ///< Permutation vector (doubled for wrapping).

    /**
     * @brief Fade function for smoothing interpolation.
     * @param T Input value between 0 and 1.
     * @return Smoothed value.
     */
    static double Fade(double T);

    /**
     * @brief Linear interpolation between two values.
     * @param T Interpolation factor.
     * @param A Start value.
     * @param B End value.
     * @return Interpolated value.
     */
    static double Lerp(double T, double A, double B);

    /**
     * @brief Computes gradient vector dot product.
     * @param Hash Hashed value used to determine gradient direction.
     * @param X X offset from grid point.
     * @param Y Y offset from grid point.
     * @return Dot product of gradient and offset vectors.
     */
    static double Grad(int Hash, double X, double Y);
};
