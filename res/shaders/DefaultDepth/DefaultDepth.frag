#version 460 core

uniform mat4 ObjectToWorldMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

in vec3 Normal;

layout (location = 0) out vec3 Color;
layout (location = 1) out vec3 OutNormal;

void main()
{
    OutNormal = normalize(Normal);
}
