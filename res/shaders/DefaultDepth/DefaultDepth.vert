#version 460 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec2 inputTexCoord;
layout (location = 2) in vec3 inputNormal;
layout (location = 3) in vec3 inputTangent;

uniform mat4 ObjectToWorldMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec4 NormalDepth;

void main()
{
    gl_Position = ProjectionMatrix * ViewMatrix * ObjectToWorldMatrix * vec4(inputPosition, 1.0);
    NormalDepth.xyz = (ObjectToWorldMatrix * vec4(inputNormal, 0.0)).xyz;
    NormalDepth.w = -(ViewMatrix * ObjectToWorldMatrix * vec4(inputPosition, 1.0)).z;
}
