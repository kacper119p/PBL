#include "Constants.glsl"

struct directionalLight
{
    mat4 DirectionalLightSpaceTransform;
    vec3 Direction;
    float _padding0;
    vec3 Color;
    float padding1;
};

struct pointLight
{
    vec3 Color;
    float LinearFalloff;
    vec3 Position;
    float QuadraticFalloff;
    float Range;
    float _padding[3];
};

struct spotLight
{
    vec3 Direction;
    float OuterAngle;
    vec3 Color;
    float InnerAngle;
    vec3 Position;
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

layout (binding = 11) uniform sampler2DShadow DirectionalLightShadowMap;
layout (binding = 12) uniform samplerCubeShadow PointLightShadowMap0;
layout (binding = 13) uniform samplerCubeShadow PointLightShadowMap1;
layout (binding = 14) uniform samplerCubeShadow SpotLightShadowMap0;
layout (binding = 15) uniform samplerCubeShadow SpotLightShadowMap1;
layout (binding = 16) uniform samplerCube IrradianceMap;
layout (binding = 17) uniform samplerCube PrefilterMap;
layout (binding = 18) uniform sampler2D BrdfLUT;
layout (binding = 19) uniform sampler2D SSAOMap;

layout (std430, binding = 0) buffer Lights {
    uint DirectionalLightCount;
    uint PointLightCount;
    uint SpotlightCount;
    directionalLight DirectionalLight;
    pointLight PointLights[2];
    spotLight SpotLights[2];
    pointLight PointLightsDynamic[];
};

const float SHADOW_BIAS = 0.00005;
const float MAX_REFLECTION_LOD = 4.0;

float FastPow5(float x)
{
    float x2 = x * x;
    return x * x2 * x2;
}

light CalculateDirectionalLight(directionalLight Light, vec3 Position)
{
    light result;
    result.Direction = Light.Direction;
    result.Color = Light.Color;

    vec4 fragPosLightSpace = DirectionalLight.DirectionalLightSpaceTransform * vec4(Position, 1.0);
    vec3 shadowCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    shadowCoords.xyz = shadowCoords.xyz * 0.5 + 0.5;
    result.Attenuation = texture(DirectionalLightShadowMap, shadowCoords, SHADOW_BIAS);
    return result;
}

light CalculatePointLight(pointLight Light, vec3 Position)
{
    light result;
    result.Color = Light.Color;

    vec3 offset = Position - Light.Position;
    float Distance = length(offset);

    result.Direction = -normalize(offset);
    result.Attenuation = 1.0 / (1.0f + Distance * (Light.LinearFalloff + Light.QuadraticFalloff * Distance));
    result.Attenuation *= step(Distance, Light.Range);

    return result;
}

light CalculatePointLightShadowed(pointLight Light, samplerCubeShadow ShadowMap, vec3 Position)
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

light CalculateSpotLight(spotLight Light, vec3 Position)
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

light CalculateSpotLightShadowed(spotLight Light, samplerCubeShadow ShadowMap, vec3 Position)
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

vec3 FresnelSchlickRoughness(float CosTheta, vec3 F0, float Roughness)
{
    return F0 + (max(vec3(1.0 - Roughness), F0) - F0) *
    FastPow5(1.0 - CosTheta);
}

vec3 FresnelSchlick(float CosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * FastPow5(1.0 - CosTheta);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return num / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 CalculateLightInfluence(light Light, vec3 BaseColor, vec3 Normal, vec3 ViewDirection, float Roughness, float Metallic, vec3 F0)
{
    vec3 H = normalize(Light.Direction + ViewDirection);
    vec3 radiance = Light.Color * Light.Attenuation;

    vec3 F = FresnelSchlick(max(dot(H, ViewDirection), 0.0), F0);

    float NDF = DistributionGGX(Normal, H, Roughness);
    float G = GeometrySmith(Normal, ViewDirection, Light.Direction, Roughness);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(Normal, ViewDirection), 0.0) * max(dot(Normal, Light.Direction), 0.0);
    vec3 specular = numerator / max(denominator, 0.001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - Metallic;

    float NdotL = max(dot(Normal, Light.Direction), 0.0);
    return (kD * BaseColor / PI + specular) * radiance * NdotL;
}

vec3 CalculateEnvironmentInfluence(vec3 BaseColor, vec3 Normal, vec3 ViewDirection, float Roughness, float Metallic, vec3 F0, float AmbientOcclusion)
{
    vec3 R = reflect(-ViewDirection, Normal);

    vec3 kS = FresnelSchlickRoughness(max(dot(Normal, ViewDirection), 0.0), F0, Roughness);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - Metallic;
    vec3 irradiance = texture(IrradianceMap, Normal).rgb;
    vec3 diffuse = irradiance * BaseColor;

    vec3 prefilteredColor = textureLod(PrefilterMap, R, Roughness *
    MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(BrdfLUT, vec2(max(dot(Normal, ViewDirection), 0.0), Roughness)).rg;
    vec3 specular = prefilteredColor * (kS * brdf.x + brdf.y);

    return (kD * diffuse + specular) * min(vec3(texture(SSAOMap, gl_FragCoord.xy / vec2(1920, 1080)).r), AmbientOcclusion);
}

vec3 CalculateLight(vec3 BaseColor, float Metallic, float Roughness, vec3 Normal, vec3 Position, vec3 ViewDirection, float AmbientOcclusion)
{
    vec3 Light = vec3(0.0, 0.0, 0.0);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, BaseColor, Metallic);

    if (DirectionalLightCount > 0)
    {
        Light += CalculateLightInfluence(CalculateDirectionalLight(DirectionalLight, Position),
                                         BaseColor,
                                         Normal,
                                         ViewDirection,
                                         Roughness,
                                         Metallic,
                                         F0);
    }

    if (PointLightCount > 0)
    {
        Light += CalculateLightInfluence(CalculatePointLightShadowed(PointLights[0], PointLightShadowMap0, Position),
                                         BaseColor,
                                         Normal,
                                         ViewDirection,
                                         Roughness,
                                         Metallic,
                                         F0);
        if (PointLightCount > 1)
        {
            Light += CalculateLightInfluence(CalculatePointLightShadowed(PointLights[1], PointLightShadowMap1, Position),
                                             BaseColor,
                                             Normal,
                                             ViewDirection,
                                             Roughness,
                                             Metallic,
                                             F0);

            for (uint i = 0; i < PointLightCount - 2; ++i)
            {
                Light += CalculateLightInfluence(CalculatePointLight(PointLightsDynamic[i], Position),
                                                 BaseColor,
                                                 Normal,
                                                 ViewDirection,
                                                 Roughness,
                                                 Metallic,
                                                 F0);
            }
        }
    }



    if (SpotlightCount > 0)
    {
        Light += CalculateLightInfluence(CalculateSpotLightShadowed(SpotLights[0], SpotLightShadowMap0, Position),
                                         BaseColor,
                                         Normal,
                                         ViewDirection,
                                         Roughness,
                                         Metallic,
                                         F0);
        if (SpotlightCount > 1)
        {
            Light += CalculateLightInfluence(CalculateSpotLightShadowed(SpotLights[1], SpotLightShadowMap1, Position),
                                             BaseColor,
                                             Normal,
                                             ViewDirection,
                                             Roughness,
                                             Metallic,
                                             F0);
        }
    }



    vec3 Color = Light + CalculateEnvironmentInfluence(BaseColor,
                                                       Normal,
                                                       ViewDirection,
                                                       Roughness,
                                                       Metallic,
                                                       F0,
                                                       AmbientOcclusion);
    return Color;
}