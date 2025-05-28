#version 460 core
#extension GL_ARB_bindless_texture: enable

#include "Lighting.glsl"
#include "Simplex.glsl"

in vec2 TexCoord;
in vec2 BloodMaskCoords;
in vec3 Position;
in vec3 Normal;
in vec3 Tangent;

uniform vec3 BaseColor;
uniform float Roughness;
uniform float Metallic;
uniform vec3 EmissiveColor;
uniform float Time;

uniform vec3 CameraPosition;

layout (bindless_sampler) uniform sampler2D BaseMap;
layout (bindless_sampler) uniform sampler2D RoughnessMetallicAmbientOcclusionMap;
layout (bindless_sampler) uniform sampler2D NormalMap;
layout (bindless_sampler) uniform sampler2D EmissiveMap;
layout (bindless_sampler) uniform sampler2D BloodNormal0;
layout (bindless_sampler) uniform sampler2D BloodNormal1;
layout (binding = 0) uniform sampler2D BloodMask;

layout (location = 0) out vec3 FragColor;
layout (location = 1) out vec3 OcclusionMask;

const float NOISE_SCALE = 30.0;
const float BLOOD_ROUGHNESS = 0.02;
const float BLOOD_METALLIC = 0.04;
const vec2 BLOOD_TILING0 = vec2(0.45, 0.45);
const vec2 BLOOD_TILING1 = vec2(0.22, 0.22);
const vec2 BLOOD_VELOCITY0 = vec2(0.035, 0.035);
const vec2 BLOOD_VELOCITY1 = vec2(-0.022, -0.022);

layout (early_fragment_tests) in;
void main() {
    vec3 BaseColor = texture(BaseMap, TexCoord).rgb * BaseColor;
    vec3 RoughnessMetallicAmbientOcclusion = texture(RoughnessMetallicAmbientOcclusionMap, TexCoord).rgb;
    float Roughness = RoughnessMetallicAmbientOcclusion.r * Roughness;
    float Metallic = RoughnessMetallicAmbientOcclusion.g * Metallic;
    float AmbientOcclusion = RoughnessMetallicAmbientOcclusion.b;

    vec4 BloodColor = texture(BloodMask, BloodMaskCoords);
    float noise = snoise(Position.xz, NOISE_SCALE);
    noise = noise * 0.5 + 0.5;
    noise *= 0.33;
    BloodColor.a = step(0.33, BloodColor.a + noise);

    vec3 Normal = normalize(Normal);
    vec3 Tangent = normalize(Tangent);
    vec3 Binormal = cross(Normal, Tangent);

    // Calculate Floor Normal
    mat3 TangentToWorldMatrix = mat3(Tangent, Binormal, Normal);
    Normal = texture(NormalMap, TexCoord).rgb * 2.0 - 1.0;
    Normal = normalize(Normal);

    // Calculate Blood Normal
    vec3 BloodNormalVector0 = texture(BloodNormal0, Position.xz * BLOOD_TILING0 + BLOOD_VELOCITY0 * Time).rgb * 2.0 - 1.0;
    vec3 BloodNormalVector1 = texture(BloodNormal1, Position.xz * BLOOD_TILING1 + BLOOD_VELOCITY1 * Time).rgb * 2.0 - 1.0;
    vec3 BloodNormal = normalize(vec3(BloodNormalVector0.xy + BloodNormalVector1.xy, BloodNormalVector0.z * BloodNormalVector1.z));
    BloodNormal = normalize(BloodNormal);

    // Lerp Floor and Blood values
    Roughness = mix(Roughness, BLOOD_ROUGHNESS, BloodColor.a);
    Metallic = mix(Metallic, BLOOD_METALLIC, BloodColor.a);
    Normal = normalize(TangentToWorldMatrix * mix(Normal, BloodNormal, BloodColor.a));
    BaseColor = mix(BaseColor, BloodColor.rgb, BloodColor.a);

    vec3 ViewDirection = normalize(CameraPosition - Position);

    vec3 Color = CalculateLight(BaseColor, Metallic, Roughness, Normal, Position, ViewDirection, AmbientOcclusion);

    vec3 Emissive = texture(EmissiveMap, TexCoord).rgb * EmissiveColor;
    Color += Emissive;

    FragColor = Color;
    OcclusionMask = Emissive;
}
