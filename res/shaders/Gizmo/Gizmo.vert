#version 460 core

layout (location = 0) in vec3 inputPosition;

uniform mat4 ObjectToWorldMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
    gl_Position = ProjectionMatrix * ViewMatrix * ObjectToWorldMatrix * vec4(inputPosition, 1.0);
}
