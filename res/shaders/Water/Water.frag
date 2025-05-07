#version 460 core
#extension GL_ARB_bindless_texture: enable

#include "Lighting.glsl"

in vec3 Position;
in vec3 Normal;
in vec3 Tangent;

uniform vec3 Color;
uniform float Roughness;
uniform float Metallic;
uniform vec2 Tiling0;
uniform vec2 Tiling1;
uniform vec2 Velocity0;
uniform vec2 Velocity1;
uniform float Time;

uniform vec3 CameraPosition;

layout (bindless_sampler) uniform sampler2D NormalMap0;
layout (bindless_sampler) uniform sampler2D NormalMap1;

out vec3 FragColor;

layout (early_fragment_tests) in;
void main() {

    vec3 Normal = normalize(Normal);
    vec3 Tangent = normalize(Tangent);
    vec3 Binormal = cross(Normal, Tangent);

    mat3 TangentToWorldMatrix = mat3(Tangent, Binormal, Normal);

    vec3 Normal0 = texture(NormalMap0, Position.xz * Tiling0 + Velocity0 * Time).rgb * 2.0 - 1.0;
    vec3 Normal1 = texture(NormalMap1, Position.xz * Tiling1 + Velocity1 * Time).rgb * 2.0 - 1.0;

    Normal = normalize(vec3(Normal0.xy + Normal1.xy, Normal0.z * Normal1.z));
    Normal = normalize(TangentToWorldMatrix * Normal);

    vec3 ViewDirection = normalize(CameraPosition - Position);

    FragColor = CalculateLight(Color, Metallic, Roughness, Normal, Position, ViewDirection, 1.0f);
}
