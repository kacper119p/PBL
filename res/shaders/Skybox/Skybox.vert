#version 460 core

layout (location = 0) in vec3 inputPosition;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;


out vec3 Direction;

void main()
{
    Direction = inputPosition;
    vec4 clipPos = ProjectionMatrix * ViewMatrix * vec4(inputPosition, 0.0);
    gl_Position = clipPos.xyww;
}