#version 460 core

in vec2 TexCoord;

uniform vec3 Color;
uniform float Fill;

layout (location = 0) out vec3 FragColor;
layout (location = 1) out vec3 OcclusionMask;

layout (early_fragment_tests) in;
void main()
{
    float mask = step(TexCoord.y, Fill - 0.01);
    FragColor = Color * mask;
    OcclusionMask = vec3(0.0);
}
