#version 460 core

struct directionalLight
{
    vec3 Direction;
    vec3 Color;
};

struct pointLight
{
    vec3 Position;
    vec3 Color;
    float LinearFalloff;
    float QuadraticFalloff;
    float Range;
};

struct spotLight
{
    vec3 Position;
    vec3 Direction;
    vec3 Color;
    float OuterAngle;
    float InnerAngle;
    float LinearFalloff;
    float QuadraticFalloff;
    float Range;
};

struct light
{
    vec3 Direction;
    vec3 Color;
    float Attenuation;
};


in vec4 Color;
in vec3 Normal;
in float Life;
in vec3 Position;

uniform vec3 CameraPosition;

layout (binding = 11) uniform sampler2DShadow DirectionalLightShadowMap;
layout (binding = 12) uniform samplerCubeShadow PointLightShadowMap0;
layout (binding = 13) uniform samplerCubeShadow PointLightShadowMap1;
layout (binding = 14) uniform samplerCubeShadow SpotLightShadowMap0;
layout (binding = 15) uniform samplerCubeShadow SpotLightShadowMap1;

uniform directionalLight DirectionalLight;
uniform mat4 DirectionalLightSpaceTransform;

uniform pointLight[8] PointLights;
uniform uint PointLightsCount;

uniform spotLight[8] SpotLights;
uniform uint SpotLightsCount;

const float PI = 3.14159265359;
const float SHADOW_BIAS = 0.00005;

out vec4 FragColor;

light CalculateDirectionalLight(directionalLight Light)
{
    light result;
    result.Direction = Light.Direction;
    result.Color = Light.Color;

    vec4 fragPosLightSpace = DirectionalLightSpaceTransform * vec4(Position, 1.0);
    vec3 shadowCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    shadowCoords.xyz = shadowCoords.xyz * 0.5 + 0.5;
    result.Attenuation = texture(DirectionalLightShadowMap, shadowCoords, SHADOW_BIAS);
    return result;
}

light CalculatePointLight(pointLight Light)
{
    light result;
    result.Color = Light.Color;

    vec3 offset = Position - Light.Position;
    float Distance = length(offset);

    result.Direction = -normalize(offset);
    result.Attenuation = 1.0 / (1.0f + Distance * (Light.LinearFalloff + Light.QuadraticFalloff * Distance));

    return result;
}

light CalculatePointLightShadowed(pointLight Light, samplerCubeShadow ShadowMap)
{
    light result;
    result.Color = Light.Color;

    vec3 offset = Position - Light.Position;
    float Distance = length(offset);

    result.Direction = -normalize(offset);
    vec4 P = vec4(offset, Distance / Light.Range);
    float ShadowAttenuation = texture(ShadowMap, P, SHADOW_BIAS);
    float DistanceAttenuation = 1.0 / (1.0f + Distance * (Light.LinearFalloff + Light.QuadraticFalloff * Distance));
    DistanceAttenuation *= step(Distance, Light.Range);

    result.Attenuation = ShadowAttenuation * DistanceAttenuation;
    return result;
}

light CalculateSpotLight(spotLight Light)
{
    light result;
    result.Color = Light.Color;

    vec3 offset = Position - Light.Position;
    float Distance = length(offset);

    result.Direction = -normalize(offset);
    vec4 P = vec4(offset, Distance / Light.Range);
    float DistanceAttenuation = 1.0 / (1.0f + Distance * (Light.LinearFalloff + Light.QuadraticFalloff * Distance));
    DistanceAttenuation *= step(Distance, Light.Range);

    float Theta = dot(result.Direction, normalize(-Light.Direction));

    float Epsilon = Light.InnerAngle - Light.OuterAngle;
    float DirectionAttenuation = clamp((Theta - Light.OuterAngle) / Epsilon, 0.0, 1.0);

    result.Attenuation = DistanceAttenuation * DirectionAttenuation;
    return result;
}

light CalculateSpotLightShadowed(spotLight Light, samplerCubeShadow ShadowMap)
{
    light result;
    result.Color = Light.Color;

    vec3 offset = Position - Light.Position;
    float Distance = length(offset);

    result.Direction = -normalize(offset);
    vec4 P = vec4(offset, Distance / Light.Range);
    float ShadowAttenuation = texture(ShadowMap, P, SHADOW_BIAS);
    float DistanceAttenuation = 1.0 / (1.0f + Distance * (Light.LinearFalloff + Light.QuadraticFalloff * Distance));
    DistanceAttenuation *= step(Distance, Light.Range);

    float Theta = dot(result.Direction, normalize(-Light.Direction));

    float Epsilon = Light.InnerAngle - Light.OuterAngle;
    float DirectionAttenuation = clamp((Theta - Light.OuterAngle) / Epsilon, 0.0, 1.0);

    result.Attenuation = ShadowAttenuation * DistanceAttenuation * DirectionAttenuation;
    return result;
}

void main() {
    vec3 LightColor = vec3(0.0, 0.0, 0.0);

    light Light = CalculateDirectionalLight(DirectionalLight);
    LightColor += Light.Color * Light.Attenuation;


    if (PointLightsCount > 0)
    {
        light Light = CalculatePointLightShadowed(PointLights[0], PointLightShadowMap0);
        LightColor += Light.Color * Light.Attenuation;
    }
    if (PointLightsCount > 1)
    {
        light Light = CalculatePointLightShadowed(PointLights[1], PointLightShadowMap1);
        LightColor += Light.Color * Light.Attenuation;
    }
    for (uint i = 2; i < PointLightsCount; ++i)
    {
        light Light = CalculatePointLight(PointLights[i]);
        LightColor += Light.Color * Light.Attenuation;
    }

    if (SpotLightsCount > 0)
    {
        light Light = CalculateSpotLightShadowed(SpotLights[0], SpotLightShadowMap0);
        LightColor += Light.Color * Light.Attenuation;
    }

    if (SpotLightsCount > 1)
    {
        light Light = CalculateSpotLightShadowed(SpotLights[1], SpotLightShadowMap1);
        LightColor += Light.Color * Light.Attenuation;
    }

    for (uint i = 2; i < SpotLightsCount; ++i)
    {
        light Light = CalculateSpotLight(SpotLights[i]);
        LightColor += Light.Color * Light.Attenuation;
    }

    LightColor *= Color.rgb;

    vec3 ViewDirection = normalize(CameraPosition - Position);
    float Fresnel = pow(clamp(dot(Normal, ViewDirection), 0, 1), 3.0);

    float Brightness = 0.299 * LightColor.r + 0.587 * LightColor.g + 0.144 * LightColor.b;

    FragColor = vec4(LightColor, clamp(Brightness, 0, 1) * Fresnel);
}