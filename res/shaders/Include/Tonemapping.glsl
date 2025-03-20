const mat3 AcesInbputMatrix =
mat3(vec3(0.59719, 0.35458, 0.04823),
vec3(0.07600, 0.90834, 0.01566),
vec3(0.02840, 0.13383, 0.83777));

const mat3 AcesOutputMatrix =
mat3(vec3(1.60475, -0.53108, -0.07367),
vec3(-0.10208, 1.10813, -0.00605),
vec3(-0.00327, -0.07276, 1.07602));

vec3 ApplyTonemapping(vec3 Color)
{
    Color *= AcesInbputMatrix;
    vec3 a = Color * (Color + 0.0245786) - 0.000090537;
    vec3 b = Color * (0.983729 * Color + 0.4329510) + 0.238081;
    Color = a / b;
    return Color * AcesOutputMatrix;
}