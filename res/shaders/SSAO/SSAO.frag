#version 460 core

layout (binding = 0) uniform sampler2DMS NormalDepth;
layout (binding = 1) uniform sampler2D texNoise;

uniform vec3 Kernel[64];
uniform mat4 ProjectionMatrix;
uniform mat4 InverseProjectionMatrix;

const vec2 SCREEN_SIZE = vec2(1920.0, 1080.0);
const vec2 NOISE_SCALE = SCREEN_SIZE / 4.0;
const float RADIUS = 3;
const float BIAS = 0.2;

out float FragColor;

vec3 GetViewPosition(vec2 uv, float viewZ) {
    float x = uv.x * 2.0 - 1.0;
    float y = uv.y * 2.0 - 1.0;
    vec4 ray_clip = vec4(x, y, -1.0, 1.0);

    vec4 ray_eye = InverseProjectionMatrix * ray_clip;
    vec3 viewDir = normalize(ray_eye.xyz);

    return 1;
}

void main() {
    vec2 texCoords = gl_FragCoord.xy / SCREEN_SIZE;

    // Fetch normal and depth
    vec4 normalDepth = texelFetch(NormalDepth, ivec2(floor(gl_FragCoord.xy)), 0);
    float depth = normalDepth.w;

    vec3 normal = normalize(normalDepth.xyz);
    vec3 fragPos = GetViewPosition(texCoords, depth);
    vec3 randomVec = normalize(texture(texNoise, texCoords * NOISE_SCALE).xyz);

    // Construct TBN matrix
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = normalize(cross(normal, tangent));
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;

    for (int i = 0; i < 64; ++i)
    {
        vec3 samplePos = fragPos + TBN * Kernel[i] * RADIUS;

        // Project to screen space
        vec4 offset = ProjectionMatrix * vec4(samplePos, 1.0);
        offset.xyz /= offset.w;
        vec2 sampleUV = offset.xy * 0.5 + 0.5;

        ivec2 sampleCoord = ivec2(clamp(sampleUV * SCREEN_SIZE, vec2(0), vec2(SCREEN_SIZE) - vec2(1)));
        float offsetDepth = texelFetch(NormalDepth, sampleCoord, 0).w;

        float sampleDepth = GetViewPosition(sampleUV, offsetDepth).z;

        float rangeCheck = smoothstep(0.0, 1.0, RADIUS / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth > samplePos.z - BIAS ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / 64.0);
    FragColor = occlusion;
}