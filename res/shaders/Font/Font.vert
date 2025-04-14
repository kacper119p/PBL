#version 460 core

layout (location = 0) in vec2 inputPosition;
layout (location = 1) in vec2 inputTexCoord;

uniform float Size;
uniform mat4 Transform;

out vec2 TexCoords;

void main()
{
    vec4 Position = vec4(inputPosition * Size, 0.0, 1.0);
    TexCoords = inputTexCoord;
    gl_Position = Transform * Position;
}