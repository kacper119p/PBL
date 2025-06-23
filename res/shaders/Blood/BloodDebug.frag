#version 460 core

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D Image;

uniform vec3 Color;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 OcclusionMask;

void main()
{
    float mask = texture(Image, TexCoords).r;
    FragColor = vec4(Color, mask);
    OcclusionMask = vec3(0.0);
}