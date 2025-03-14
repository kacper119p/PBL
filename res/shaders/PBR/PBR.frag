#version 460 core

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

layout (binding = 0) uniform sampler2D BaseMap;
layout (binding = 1) uniform sampler2D RoughnessMetallicAmbientOcclusionMap;
layout (binding = 2) uniform sampler2D NormalMap;
layout (binding = 3) uniform sampler2D EmissiveMap;

out vec3 FragColor;

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

    vec3 Light = vec3(0.0, 0.0, 0.0);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, BaseColor, Metallic);

    Light += CalculateLightInfluence(CalculateDirectionalLight(DirectionalLight, Position),
                                     BaseColor,
                                     Normal,
                                     ViewDirection,
                                     Roughness,
                                     Metallic,
                                     F0);


    if (PointLightsCount > 0)
    {
        Light += CalculateLightInfluence(CalculatePointLightShadowed(PointLights[0], PointLightShadowMap0, Position),
                                         BaseColor,
                                         Normal,
                                         ViewDirection,
                                         Roughness,
                                         Metallic,
                                         F0);
    }
    if (PointLightsCount > 1)
    {
        Light += CalculateLightInfluence(CalculatePointLightShadowed(PointLights[1], PointLightShadowMap1, Position),
                                         BaseColor,
                                         Normal,
                                         ViewDirection,
                                         Roughness,
                                         Metallic,
                                         F0);
    }
    for (uint i = 2; i < PointLightsCount; ++i)
    {
        Light += CalculateLightInfluence(CalculatePointLight(PointLights[i], Position),
                                         BaseColor,
                                         Normal,
                                         ViewDirection,
                                         Roughness,
                                         Metallic,
                                         F0);
    }

    if (SpotLightsCount > 0)
    {
        Light += CalculateLightInfluence(CalculateSpotLightShadowed(SpotLights[0], SpotLightShadowMap0, Position),
                                         BaseColor,
                                         Normal,
                                         ViewDirection,
                                         Roughness,
                                         Metallic,
                                         F0);

    }

    if (SpotLightsCount > 1)
    {
        Light += CalculateLightInfluence(CalculateSpotLightShadowed(SpotLights[1], SpotLightShadowMap1, Position),
                                         BaseColor,
                                         Normal,
                                         ViewDirection,
                                         Roughness,
                                         Metallic,
                                         F0);

    }

    for (uint i = 2; i < SpotLightsCount; ++i)
    {
        Light += CalculateLightInfluence(CalculateSpotLight(SpotLights[i], Position),
                                         BaseColor,
                                         Normal,
                                         ViewDirection,
                                         Roughness,
                                         Metallic,
                                         F0);
    }

    vec3 Color = Light + CalculateEnvironmentInfluence(BaseColor,
                                                       Normal,
                                                       ViewDirection,
                                                       Roughness,
                                                       Metallic,
                                                       F0,
                                                       AmbientOcclusion);

    vec3 Emissive = texture(EmissiveMap, TexCoord).rgb * EmissiveColor;
    Color += Emissive;

    FragColor = Color;
}
