#version 460 core
#extension GL_ARB_bindless_texture: enable

in vec2 TexCoord;
in vec3 Position;
in vec3 WorldNormal;

uniform float Time;
uniform float Visibility;
uniform vec3 CameraLocation;

layout (bindless_sampler) uniform sampler2D Texture;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 OcclusionMask;

layout (early_fragment_tests) in;
void main()
{
    float alpha = texture(Texture, TexCoord + vec2(Time, -Time) * 1.25f).r;
    float edgeFade = smoothstep(0.0, 0.1, TexCoord.y) * smoothstep(1.0, 0.75, TexCoord.y);
    float oneMinusVisibility = 1.0 - Visibility;
    float visibilityFade = smoothstep(oneMinusVisibility - 0.1, oneMinusVisibility + 0.1, TexCoord.y);
    vec3 ViewDir = normalize(CameraLocation - Position);
    float fresnel = pow((1.0 - clamp(dot(normalize(WorldNormal), ViewDir), 0, 1)), 1.0);
    alpha *= fresnel * edgeFade * visibilityFade;
    FragColor = vec4(vec3(1.0), alpha);
    OcclusionMask = vec4(vec3(0.0), alpha);
}
