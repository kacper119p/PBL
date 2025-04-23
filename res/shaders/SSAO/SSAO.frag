#version 460 core

layout (binding = 0) uniform sampler2D NormalDepth;
layout (binding = 1) uniform sampler2D texNoise;

uniform vec3 Kernel[64];
uniform mat4 ProjectionMatrix;
uniform mat4 InverseProjectionMatrix;

in vec2 TexCoords;

out float FragColor;

const vec2 SCREEN_SIZE = vec2(1920.0 / 2.0, 1080.0 / 2.0);
const vec2 NOISE_SCALE = SCREEN_SIZE / 4.0;
const float RADIUS = 1.5;
const float BIAS = 0.015;

vec3 GetViewPosition(vec2 uv, float viewZ) {
    float x = uv.x * 2.0 - 1.0;
    float y = uv.y * 2.0 - 1.0;
    vec4 ray_clip = vec4(x, y, -1.0, 1.0);

    vec4 ray_eye = InverseProjectionMatrix * ray_clip;
    vec3 viewDir = normalize(ray_eye.xyz);

    return viewDir * viewZ;
}

void main() {
    // Fetch normal and depth
    vec4 normalDepth = texture(NormalDepth, TexCoords);
    float depth = normalDepth.w;

    vec3 normal = normalize(normalDepth.xyz);
    vec3 fragPos = GetViewPosition(TexCoords, depth);
    vec3 randomVec = normalize(texture(texNoise, TexCoords * NOISE_SCALE).xyz);

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

        float offsetDepth = texture(NormalDepth, sampleUV).w;

        float sampleDepth = GetViewPosition(sampleUV, offsetDepth).z;

        float rangeCheck = smoothstep(0.0, 1.0, RADIUS / abs(fragPos.z - sampleDepth));
        occlusion += step(samplePos.z - BIAS, sampleDepth) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / 64.0);
    FragColor = occlusion;
}