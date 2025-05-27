#version 460 core
#extension GL_ARB_bindless_texture: enable

#include "Lighting.glsl"

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

layout (std140, binding = 0) uniform MaterialData
{
    float BloodRoughness;
    float BloodMetallic;
    vec2 BloodTiling0;
    vec2 BloodTiling1;
    vec2 BloodVelocity0;
    vec2 BloodVelocity1;
};

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

layout (early_fragment_tests) in;
void main() {
    vec3 BaseColor = texture(BaseMap, TexCoord).rgb * BaseColor;
    vec3 RoughnessMetallicAmbientOcclusion = texture(RoughnessMetallicAmbientOcclusionMap, TexCoord).rgb;
    float Roughness = RoughnessMetallicAmbientOcclusion.r * Roughness;
    float Metallic = RoughnessMetallicAmbientOcclusion.g * Metallic;
    float AmbientOcclusion = RoughnessMetallicAmbientOcclusion.b;

    vec4 BloodColor = texture(BloodMask, BloodMaskCoords);

    vec3 Normal = normalize(Normal);
    vec3 Tangent = normalize(Tangent);
    vec3 Binormal = cross(Normal, Tangent);

    // Calculate Floor Normal
    mat3 TangentToWorldMatrix = mat3(Tangent, Binormal, Normal);
    Normal = texture(NormalMap, TexCoord).rgb * 2.0 - 1.0;
    Normal = normalize(TangentToWorldMatrix * Normal);

    // Calculate Blood Normal
    vec3 BloodNormalVector0 = texture(BloodNormal0, Position.xz * BloodTiling0 + BloodVelocity0 * Time).rgb * 2.0 - 1.0;
    vec3 BloodNormalVector1 = texture(BloodNormal1, Position.xz * BloodTiling1 + BloodVelocity1 * Time).rgb * 2.0 - 1.0;
    vec3 BloodNormal = normalize(vec3(BloodNormalVector0.xy + BloodNormalVector1.xy, BloodNormalVector0.z * BloodNormalVector1.z));
    BloodNormal = normalize(TangentToWorldMatrix * BloodNormal);

    // Lerp Floor and Blood values
    Roughness = mix(Roughness, BloodRoughness, BloodColor.a);
    Metallic = mix(Metallic, BloodMetallic, BloodColor.a);
    Normal = mix(Normal, BloodNormal, BloodColor.a);
    BaseColor = mix(BaseColor, BloodColor.rgb, BloodColor.a);

    vec3 ViewDirection = normalize(CameraPosition - Position);

    vec3 Color = CalculateLight(BaseColor, Metallic, Roughness, Normal, Position, ViewDirection, AmbientOcclusion);

    vec3 Emissive = texture(EmissiveMap, TexCoord).rgb * EmissiveColor;
    Color += Emissive;

    FragColor = Color;
    OcclusionMask = Emissive;
}
