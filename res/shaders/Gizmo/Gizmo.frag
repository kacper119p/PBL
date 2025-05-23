#version 460 core

layout (location = 0) out vec3 FragColor;
layout (location = 1) out vec3 OcclusionMask;

void main()
{
    FragColor = vec3(0.0, 15.0f, 0.0f);
    OcclusionMask = vec3(0.0);
}