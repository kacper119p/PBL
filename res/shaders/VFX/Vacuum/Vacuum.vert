#version 460 core

#include "Simplex.glsl"

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec2 inputTexCoord;
layout (location = 2) in vec3 inputNormal;
layout (location = 3) in vec3 inputTangent;

out vec2 TexCoord;

uniform float Time;

uniform mat4 ObjectToWorldMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
    TexCoord = inputTexCoord;
    vec3 Position = inputPosition + snoise(inputPosition + vec3(Time, Time, -Time) * 0.5f, 10f) * 0.1f * inputNormal;
    gl_Position = ProjectionMatrix * ViewMatrix * ObjectToWorldMatrix * vec4(Position, 1.0);
}
