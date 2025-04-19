#version 460 core

#include "Lighting.glsl"

in vec2 TexCoord;
in vec3 Position;
flat in vec3 Normal;

uniform vec3 BaseColor;
uniform float Roughness;
uniform float Metallic;
uniform vec3 EmissiveColor;

uniform vec3 CameraPosition;

layout (binding = 0) uniform sampler2D BaseMap;
layout (binding = 1) uniform sampler2D RoughnessMetallicAmbientOcclusionMap;
layout (binding = 3) uniform sampler2D EmissiveMap;

out vec3 FragColor;

void main() {
    vec3 baseColor = texture(BaseMap, TexCoord).rgb * BaseColor;
    vec3 rma = texture(RoughnessMetallicAmbientOcclusionMap, TexCoord).rgb;
    float roughness = rma.r * Roughness;
    float metallic = rma.g * Metallic;
    float ao = rma.b;

    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(CameraPosition - Position);

    vec3 light = vec3(0.0);
    vec3 f0 = mix(vec3(0.04), baseColor, metallic);

    light += CalculateLightInfluence(CalculateDirectionalLight(DirectionalLight, Position),
                                     baseColor, normal, viewDir, roughness, metallic, f0);

    for (uint i = 0; i < PointLightsCount; ++i) {
        light += CalculateLightInfluence(CalculatePointLight(PointLights[i], Position),
                                         baseColor, normal, viewDir, roughness, metallic, f0);
    }

    for (uint i = 0; i < SpotLightsCount; ++i) {
        light += CalculateLightInfluence(CalculateSpotLight(SpotLights[i], Position),
                                         baseColor, normal, viewDir, roughness, metallic, f0);
    }

    vec3 color = light + CalculateEnvironmentInfluence(baseColor, normal, viewDir, roughness, metallic, f0, ao);
    vec3 emissive = texture(EmissiveMap, TexCoord).rgb * EmissiveColor;
    color += emissive;

    FragColor = color;
}
