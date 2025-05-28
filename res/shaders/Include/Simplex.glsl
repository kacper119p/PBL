vec3 mod289(vec3 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
vec2 mod289(vec2 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
vec3 permute(vec3 x) {
    return mod289((x * 34.0 + 1.0) * x);
}
float snoise(vec2 uv, float scale) {
    uv *= scale;
    const vec4 C = vec4(0.211324865405187, // (3.0 - sqrt(3.0)) / 6.0
    0.366025403784439, // 0.5 * (sqrt(3.0) - 1.0)
    -0.577350269189626, // -1.0 + 2.0 * C.x
    0.024390243902439); // 1.0 / 41.0

    // First corner
    vec2 i = floor(uv + dot(uv, C.yy));
    vec2 x0 = uv - i + dot(i, C.xx);

    // Other two corners
    vec2 i1;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec2 x1 = x0.xy - i1 + C.xx;
    vec2 x2 = x0.xy - 1.0 + 2.0 * C.xx;

    // Permutations
    i = mod289(i);
    vec3 p = permute(permute(i.y + vec3(0.0, i1.y, 1.0))
                     + i.x + vec3(0.0, i1.x, 1.0));

    // Gradients
    vec3 x_ = fract(p * C.w) * 2.0 - 1.0;
    vec3 h = abs(x_) - 0.5;
    vec3 ox = floor(x_ + 0.5);
    vec3 a0 = x_ - ox;

    // Final gradients
    vec2 g0 = vec2(a0.x, h.x);
    vec2 g1 = vec2(a0.y, h.y);
    vec2 g2 = vec2(a0.z, h.z);

    // Compute noise contributions from each corner
    vec3 t = 0.5 - vec3(dot(x0, x0), dot(x1, x1), dot(x2, x2));
    vec3 m = max(t, 0.0);
    m = m * m;
    m = m * m;

    vec3 dotProd = vec3(dot(g0, x0), dot(g1, x1), dot(g2, x2));

    return 70.0 * dot(m, dotProd);
}