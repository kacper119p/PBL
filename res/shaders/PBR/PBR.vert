#version 460 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec2 inputTexCoord;
layout (location = 2) in vec3 inputNormal;
layout (location = 3) in vec3 inputTangent;

out vec2 TexCoord;
out vec3 Position;
out vec3 Normal;
out vec3 Tangent;

uniform mat4 ObjectToWorldMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
    gl_Position = ProjectionMatrix * ViewMatrix * ObjectToWorldMatrix * vec4(inputPosition, 1.0);

    Position = (ObjectToWorldMatrix * vec4(inputPosition, 1.0)).xyz;
    TexCoord = inputTexCoord;
    Normal = (ObjectToWorldMatrix * vec4(inputNormal, 0.0)).xyz;
    Tangent = (ObjectToWorldMatrix * vec4(inputTangent, 0.0)).xyz;
}
