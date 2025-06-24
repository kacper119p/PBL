#version 460 core

uniform vec4 Tint;

layout (location = 0) out vec4 FragColor;

void main()
{
    FragColor = Tint;
}