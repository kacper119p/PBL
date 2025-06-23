#version 460 core

layout (location = 0) in vec2 inputPosition;

uniform float Size;

out vec2 TexCoords;

void main()
{
    TexCoords = inputPosition;
    gl_Position = vec4((inputPosition * 2.0 - 1.0) * Size, 0.0f, 1.0);
}