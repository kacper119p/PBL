vec3 mod289(vec3 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
vec3 permute(vec3 x)
{
    return mod289((x * 34.0 + 1.0) * x);
}
vec4 permute(vec4 x)
{
    return mod289((x * 34.0 + 1.0) * x);
}

// --- 1D Simplex Noise ---
float snoise(float uv, float scale)
{
    uv *= scale;
    const float C = 0.211324865405187;  // (3 - sqrt(3)) / 6
    float i0 = floor(uv + 0.5);
    float x0 = uv - i0 + 0.5;

    float i1 = x0 > 0.5 ? 1.0 : 0.0;
    float x1 = x0 - i1 + C;
    float x2 = x0 - 1.0 + 2.0 * C;

    vec3 x_vals = vec3(x0, x1, x2);
    vec3 t = 0.5 - x_vals * x_vals;
    vec3 m = max(t, 0.0);
    m = m * m;

    vec3 p = permute(vec3(i0, i0 + i1, i0 + 1.0));
    vec3 grad = fract(p * 0.024390243902439) * 2.0 - 1.0;

    vec3 dot_vals = grad * x_vals;
    return 42.0 * dot(m, dot_vals);
}

// --- 2D Simplex Noise ---
float snoise(vec2 uv, float scale)
{
    uv *= scale;
    const vec4 C = vec4(0.211324865405187, 0.366025403784439,
    -0.577350269189626, 0.024390243902439);
    vec2 i = floor(uv + dot(uv, C.yy));
    vec2 x0 = uv - i + dot(i, C.xx);
    vec2 i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec2 x1 = x0.xy - i1 + C.xx;
    vec2 x2 = x0.xy - 1.0 + 2.0 * C.xx;

    i = mod289(i);
    vec3 p = permute(permute(i.y + vec3(0.0, i1.y, 1.0))
                     + i.x + vec3(0.0, i1.x, 1.0));

    vec3 x_ = fract(p * C.w) * 2.0 - 1.0;
    vec3 h = abs(x_) - 0.5;
    vec3 ox = floor(x_ + 0.5);
    vec3 a0 = x_ - ox;

    vec2 g0 = vec2(a0.x, h.x);
    vec2 g1 = vec2(a0.y, h.y);
    vec2 g2 = vec2(a0.z, h.z);

    vec3 t = 0.5 - vec3(dot(x0, x0), dot(x1, x1), dot(x2, x2));
    vec3 m = max(t, 0.0);
    m = m * m;
    m = m * m;

    vec3 dotProd = vec3(dot(g0, x0), dot(g1, x1), dot(g2, x2));
    return 70.0 * dot(m, dotProd);
}

// --- 3D Simplex Noise ---
float snoise(vec3 uv, float scale)
{
    uv *= scale;
    const vec2 C = vec2(1.0 / 6.0, 1.0 / 3.0);
    const float D = 0.142857142857; // 1/7

    vec3 i = floor(uv + dot(uv, C.yyy));
    vec3 x0 = uv - i + dot(i, C.xxx);

    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min(g.xyz, l.zxy);
    vec3 i2 = max(g.xyz, l.zxy);

    vec3 x1 = x0 - i1 + C.xxx;
    vec3 x2 = x0 - i2 + C.yyy;
    vec3 x3 = x0 - 1.0 + 3.0 * C.xxx;

    i = mod289(i);
    vec4 p = permute(permute(permute(
                                 i.z + vec4(0.0, i1.z, i2.z, 1.0)) +
                             i.y + vec4(0.0, i1.y, i2.y, 1.0)) +
                     i.x + vec4(0.0, i1.x, i2.x, 1.0));

    vec4 j = p - 49.0 * floor(p * D * D);  // mod(p,7*7)

    vec4 x_ = floor(j * D);
    vec4 y_ = floor(j - 7.0 * x_);
    vec4 x = (x_ * 2.0 + 0.5) / 7.0 - 1.0;
    vec4 y = (y_ * 2.0 + 0.5) / 7.0 - 1.0;

    vec4 h = 1.0 - abs(x) - abs(y);
    vec4 b0 = vec4(x.xy, y.xy);
    vec4 b1 = vec4(x.zw, y.zw);

    vec4 s0 = floor(b0) * 2.0 + 1.0;
    vec4 s1 = floor(b1) * 2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));

    vec4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
    vec4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

    vec3 g0 = vec3(a0.xy, h.x);
    vec3 g1 = vec3(a0.zw, h.y);
    vec3 g2 = vec3(a1.xy, h.z);
    vec3 g3 = vec3(a1.zw, h.w);

    vec4 norm = inversesqrt(vec4(dot(g0, g0), dot(g1, g1), dot(g2, g2), dot(g3, g3)));
    g0 *= norm.x;
    g1 *= norm.y;
    g2 *= norm.z;
    g3 *= norm.w;

    vec4 m = max(0.6 - vec4(dot(x0, x0), dot(x1, x1),
    dot(x2, x2), dot(x3, x3)), 0.0);
    m = m * m;
    return 42.0 * dot(m * m, vec4(dot(g0, x0), dot(g1, x1),
    dot(g2, x2), dot(g3, x3)));
}