#version 460 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec2 inputTexCoord;
layout (location = 2) in vec3 inputNormal;
layout (location = 3) in vec3 inputTangent;

uniform float OutlineSize;

uniform mat4 ObjectToWorldMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
    vec3 worldPosition = (ObjectToWorldMatrix * vec4(inputPosition, 1.0)).xyz;
    worldPosition += (ObjectToWorldMatrix * vec4(inputNormal, 0.0)).xyz * OutlineSize;
    gl_Position = ProjectionMatrix * ViewMatrix * vec4(worldPosition, 1.0);
}
