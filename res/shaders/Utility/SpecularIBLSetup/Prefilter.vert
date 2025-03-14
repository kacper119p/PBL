#version 460 core

layout (location = 0) in vec3 inputPosition;

out vec3 LocalPosition;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

void main()
{
    LocalPosition = inputPosition;
    gl_Position = ProjectionMatrix * ViewMatrix * vec4(inputPosition, 1.0);
}