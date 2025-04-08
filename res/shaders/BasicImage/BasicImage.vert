#version 460 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec2 inputTexCoord;

uniform mat4 Transform;

out vec2 TexCoords;

void main()
{
    TexCoords = inputTexCoord;
    gl_Position = Transform * vec4(inputPosition, 1.0);
}