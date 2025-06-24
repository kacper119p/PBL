#version 460 core
#extension GL_ARB_bindless_texture: enable

#include "Simplex.glsl"

in vec2 UV;

layout (bindless_sampler) uniform sampler2D Sprite;

uniform vec3 Color;
uniform float Time;
/*
* xy - speed
* zw - scale
*/
uniform vec4 NoiseParameters;
uniform vec2 NoiseStrength;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 OcclusionMask;

layout (early_fragment_tests) in;
void main()
{
    vec2 uv = UV + snoise(UV * NoiseParameters.zw + NoiseParameters.xy * Time, 1.0) * NoiseStrength * UV.y;
    float alpha = texture(Sprite, uv).r;
    FragColor = vec4(Color, alpha);
    OcclusionMask = vec4(vec3(0.0), alpha);
}
