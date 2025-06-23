#version 460 core

#include "VacuumShotData.glsl"

layout (std430, binding = 1) readonly restrict buffer Particles {
    particle particles[];
};

layout (location = 0) in vec2 inputTexCoord;

out vec2 UV;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ObjectToWorldMatrix;

void main()
{
    particle particleData = particles[gl_InstanceID];

    vec3 velocity = vec3(normalize(ObjectToWorldMatrix * vec4(particleData.velocity, 0.0)));

    // Fallback in case velocity is zero
    if (length(velocity) < 0.0001)
    velocity = vec3(0.0, 1.0, 0.0);

    //Bilboarding - align along velocity
    vec3 worldPosition = (ObjectToWorldMatrix * vec4(particleData.position, 1.0)).xyz;

    vec3 cameraForward = - vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);

    vec3 refVec = abs(dot(velocity, cameraForward)) > 0.99 ? vec3(0.0, 1.0, 0.0) : cameraForward;

    vec3 billboardRight = normalize(cross(cameraForward, velocity));
    vec3 billboardUp = velocity;

    vec3 billboardOffset = ((inputTexCoord.x) * billboardRight + (inputTexCoord.y - 0.5) * billboardUp * 4.0) * particleData.scale;

    vec3 finalPos = worldPosition + billboardOffset;

    gl_Position = ProjectionMatrix * ViewMatrix * vec4(finalPos, 1.0);

    UV = inputTexCoord;
}
