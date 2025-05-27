#version 460 core

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D Image;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 OcclusionMask;

void main()
{
    FragColor = texture(Image, TexCoords);
    OcclusionMask = vec3(0.0);
}