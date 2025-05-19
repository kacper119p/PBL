#version 460 core
#extension GL_ARB_bindless_texture: enable

#include "Lighting.glsl"

in vec2 TexCoord;
in vec3 Position;
in vec3 Normal;
in vec3 Tangent;

uniform vec3 BaseColor;
uniform float Roughness;
uniform float Metallic;
uniform vec3 EmissiveColor;

uniform vec3 CameraPosition;

layout (bindless_sampler) uniform sampler2D BaseMap;
layout (bindless_sampler) uniform sampler2D RoughnessMetallicAmbientOcclusionMap;
layout (bindless_sampler) uniform sampler2D NormalMap;
layout (bindless_sampler) uniform sampler2D EmissiveMap;

layout (location = 0) out vec3 FragColor;
layout (location = 1) out vec3 OcclusionMask;

layout (early_fragment_tests) in;
void main() {
    vec3 BaseColor = texture(BaseMap, TexCoord).rgb * BaseColor;
    vec3 RoughnessMetallicAmbientOcclusion = texture(RoughnessMetallicAmbientOcclusionMap, TexCoord).rgb;
    float Roughness = RoughnessMetallicAmbientOcclusion.r * Roughness;
    float Metallic = RoughnessMetallicAmbientOcclusion.g * Metallic;
    float AmbientOcclusion = RoughnessMetallicAmbientOcclusion.b;

    vec3 Normal = normalize(Normal);
    vec3 Tangent = normalize(Tangent);
    vec3 Binormal = cross(Normal, Tangent);

    mat3 TangentToWorldMatrix = mat3(Tangent, Binormal, Normal);
    Normal = texture(NormalMap, TexCoord).rgb * 2.0 - 1.0;
    Normal = normalize(TangentToWorldMatrix * Normal);

    vec3 ViewDirection = normalize(CameraPosition - Position);

    vec3 Color = CalculateLight(BaseColor, Metallic, Roughness, Normal, Position, ViewDirection, AmbientOcclusion);

    vec3 Emissive = texture(EmissiveMap, TexCoord).rgb * EmissiveColor;
    Color += Emissive;

    FragColor = Color;
    OcclusionMask = Emissive;
}
