#version 460 core

#include "ExhaustParticleData.glsl"

layout (std430, binding = 1) readonly restrict buffer Particles {
    particle particles[];
};

layout (std430, binding = 2) readonly buffer SortedIndices {
    int sortedIndices[];
};


layout (location = 0) in vec2 inputTexCoord;

out vec2 UV;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ObjectToWorldMatrix;

void main()
{
    particle particleData = particles[sortedIndices[gl_InstanceID]];

    //Bilboarding
    vec3 cameraRight = vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
    vec3 cameraUp = vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

    vec3 worldPosition = (ObjectToWorldMatrix * vec4(particleData.position, 1.0)).xyz;

    vec2 localOffset = (inputTexCoord - vec2(0.5)) * particleData.scale;

    float cosTheta = cos(particleData.rotation);
    float sinTheta = sin(particleData.rotation);

    vec2 rotatedOffset;
    rotatedOffset.x = localOffset.x * cosTheta - localOffset.y * sinTheta;
    rotatedOffset.y = localOffset.x * sinTheta + localOffset.y * cosTheta;

    vec3 billboardOffset = rotatedOffset.x * cameraRight + rotatedOffset.y * cameraUp;

    vec3 finalPos = worldPosition + billboardOffset;

    gl_Position = ProjectionMatrix * ViewMatrix * vec4(finalPos, 1.0);

    UV = inputTexCoord;
}
