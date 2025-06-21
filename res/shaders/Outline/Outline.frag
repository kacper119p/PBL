#version 460 core
#extension GL_ARB_bindless_texture: enable

#include "Lighting.glsl"

in vec2 TexCoord;
in vec3 Position;
in vec3 Normal;
in vec3 Tangent;

uniform vec3 Color;

layout (location = 0) out vec3 FragColor;
layout (location = 1) out vec3 OcclusionMask;

layout (early_fragment_tests) in;
void main()
{
    FragColor = Color;
    OcclusionMask = vec3(0.0);
}
