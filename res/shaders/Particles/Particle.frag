#version 460 core

#include "Lighting.glsl"

in vec4 Color;
in vec3 Normal;
in float Life;
in vec3 Position;

uniform vec3 CameraPosition;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 OcclusionMask;

layout (early_fragment_tests) in;
void main() {
    vec3 LightColor = vec3(0.0, 0.0, 0.0);

    light Light = CalculateDirectionalLight(DirectionalLight, Position);
    LightColor += Light.Color * Light.Attenuation;

    Light = CalculatePointLightShadowed(PointLights[0], PointLightShadowMap0, Position);
    LightColor += Light.Color * Light.Attenuation;

    Light = CalculatePointLightShadowed(PointLights[1], PointLightShadowMap1, Position);
    LightColor += Light.Color * Light.Attenuation;

    Light = CalculateSpotLightShadowed(SpotLights[0], SpotLightShadowMap0, Position);
    LightColor += Light.Color * Light.Attenuation;

    Light = CalculateSpotLightShadowed(SpotLights[1], SpotLightShadowMap1, Position);
    LightColor += Light.Color * Light.Attenuation;

    for (uint i = 0; i < PointLightsDynamic.length(); ++i)
    {
        Light = CalculatePointLight(PointLightsDynamic[i], Position);
        LightColor += Light.Color * Light.Attenuation;
    }

    LightColor *= Color.rgb;

    vec3 ViewDirection = normalize(CameraPosition - Position);
    float Fresnel = pow(clamp(dot(Normal, ViewDirection), 0, 1), 3.0);

    float Brightness = 0.299 * LightColor.r + 0.587 * LightColor.g + 0.144 * LightColor.b;

    FragColor = vec4(LightColor, clamp(Brightness, 0, 1) * Fresnel);
    OcclusionMask = vec3(0.0);
}