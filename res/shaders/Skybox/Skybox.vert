#version 460 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec2 inputTexCoord;
layout (location = 2) in vec3 inputNormal;
layout (location = 3) in vec3 inputTangent;

uniform mat4 ObjectToWorldMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;


out vec3 Direction;

void main()
{
    Direction = inputPosition;
    vec4 clipPos = ProjectionMatrix * mat4(mat3(ViewMatrix)) * vec4(inputPosition, 1.0);
    gl_Position = clipPos.xyww;
}