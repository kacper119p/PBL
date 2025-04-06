#version 460 core

layout (location = 0) in vec2 inputPosition;
layout (location = 1) in vec2 inputTexCoord;

out vec2 TexCoords;

void main()
{
    vec4 Position = vec4(inputPosition / 5, 0.0, 1.0);
    TexCoords = inputTexCoord;
    gl_Position = Position;
}