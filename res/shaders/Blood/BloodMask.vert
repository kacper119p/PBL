#version 460 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec2 inputTexCoord;

uniform mat4 ViewProjectionMatrix;
uniform mat4 ModelMatrix;

out vec2 TexCoords;

void main()
{
    TexCoords = inputTexCoord;
    gl_Position = ViewProjectionMatrix * ModelMatrix * vec4(inputPosition, 1.0);
}