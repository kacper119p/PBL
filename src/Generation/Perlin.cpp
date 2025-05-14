#include "Perlin.h"
#include <numeric>
#include <algorithm>
#include <cmath>

Perlin::Perlin(unsigned int Seed)
{
    P.resize(256);
    std::iota(P.begin(), P.end(), 0);

    std::default_random_engine engine(Seed);
    std::shuffle(P.begin(), P.end(), engine);

    P.insert(P.end(), P.begin(), P.end());
}

double Perlin::Noise(double X, double Y) const
{
    int x = static_cast<int>(std::floor(X)) & 255;
    int y = static_cast<int>(std::floor(Y)) & 255;

    X -= std::floor(X);
    Y -= std::floor(Y);

    double u = Fade(X);
    double v = Fade(Y);

    int a = P[x] + y;
    int b = P[x + 1] + y;

    return Lerp(v,
                Lerp(u, Grad(P[a], X, Y), Grad(P[b], X - 1, Y)),
                Lerp(u, Grad(P[a + 1], X, Y - 1), Grad(P[b + 1], X - 1, Y - 1)));
}

double Perlin::Fade(double T)
{
    return T * T * T * (T * (T * 6 - 15) + 10);
}

double Perlin::Lerp(double T, double A, double B)
{
    return A + T * (B - A);
}

double Perlin::Grad(int Hash, double X, double Y)
{
    int h = Hash & 7;
    double u = h < 4 ? X : Y;
    double v = h < 4 ? Y : X;
    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0 * v : 2.0 * v);
}
