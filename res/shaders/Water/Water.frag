#version 460 core

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

layout (binding = 0) uniform sampler2D NormalMap0;
layout (binding = 1) uniform sampler2D NormalMap1;

out vec3 FragColor;

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

    vec3 Light = vec3(0.0, 0.0, 0.0);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Color, Metallic);

    Light += CalculateLightInfluence(CalculateDirectionalLight(DirectionalLight, Position),
                                     Color,
                                     Normal,
                                     ViewDirection,
                                     Roughness,
                                     Metallic,
                                     F0);


    if (PointLightsCount > 0)
    {
        Light += CalculateLightInfluence(CalculatePointLightShadowed(PointLights[0], PointLightShadowMap0, Position),
                                         Color,
                                         Normal,
                                         ViewDirection,
                                         Roughness,
                                         Metallic,
                                         F0);
    }
    if (PointLightsCount > 1)
    {
        Light += CalculateLightInfluence(CalculatePointLightShadowed(PointLights[1], PointLightShadowMap1, Position),
                                         Color,
                                         Normal,
                                         ViewDirection,
                                         Roughness,
                                         Metallic,
                                         F0);
    }
    for (uint i = 2; i < PointLightsCount; ++i)
    {
        Light += CalculateLightInfluence(CalculatePointLight(PointLights[i], Position),
                                         Color,
                                         Normal,
                                         ViewDirection,
                                         Roughness,
                                         Metallic,
                                         F0);
    }

    if (SpotLightsCount > 0)
    {
        Light += CalculateLightInfluence(CalculateSpotLightShadowed(SpotLights[0], SpotLightShadowMap0, Position),
                                         Color,
                                         Normal,
                                         ViewDirection,
                                         Roughness,
                                         Metallic,
                                         F0);

    }

    if (SpotLightsCount > 1)
    {
        Light += CalculateLightInfluence(CalculateSpotLightShadowed(SpotLights[1], SpotLightShadowMap1, Position),
                                         Color,
                                         Normal,
                                         ViewDirection,
                                         Roughness,
                                         Metallic,
                                         F0);

    }

    for (uint i = 2; i < SpotLightsCount; ++i)
    {
        Light += CalculateLightInfluence(CalculateSpotLight(SpotLights[i], Position),
                                         Color,
                                         Normal,
                                         ViewDirection,
                                         Roughness,
                                         Metallic,
                                         F0);
    }

    vec3 Color = Light + CalculateEnvironmentInfluence(Color,
                                                       Normal,
                                                       ViewDirection,
                                                       Roughness,
                                                       Metallic,
                                                       F0,
                                                       1.0);

    FragColor = Color;
}
