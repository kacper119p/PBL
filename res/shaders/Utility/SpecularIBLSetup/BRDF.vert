#version 460 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec2 inputTexCoord;

out vec2 TexCoords;

void main()
{
    TexCoords = inputTexCoord;
    gl_Position = vec4(inputPosition, 1.0);
}