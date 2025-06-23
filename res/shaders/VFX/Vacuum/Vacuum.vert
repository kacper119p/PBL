#version 460 core

#include "Simplex.glsl"

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec2 inputTexCoord;
layout (location = 2) in vec3 inputNormal;
layout (location = 3) in vec3 inputTangent;

out vec2 TexCoord;
out vec3 Position;
out vec3 WorldNormal;

uniform float Time;

uniform mat4 ObjectToWorldMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

vec3 displace(vec3 Position, vec3 Normal, float Scale, float Strength, float Velocity)
{
    float displacement = snoise(Position + vec3(Time, Time, -Time) * Velocity, Scale);
    return displacement * Strength * Normal;
}

vec3 displace(vec3 Position, vec3 Normal)
{
    vec3 displacement = displace(inputPosition, inputNormal, 10.0, 0.15, 0.5);
    displacement += displace(inputPosition, inputNormal, 1.0, 0.3, 2.0);
    return Position + displacement * (1.0 - inputTexCoord.y);
}

void main()
{
    TexCoord = inputTexCoord;

    vec3 displacedPosition = displace(inputPosition, inputNormal);

    WorldNormal = (ObjectToWorldMatrix * vec4(inputNormal, 0.0)).xyz;

    Position = (ObjectToWorldMatrix * vec4(displacedPosition, 1.0)).xyz;
    gl_Position = ProjectionMatrix * ViewMatrix * ObjectToWorldMatrix * vec4(displacedPosition, 1.0);
}
