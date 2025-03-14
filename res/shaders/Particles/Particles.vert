#version 460 core

struct particle {
    vec3 position;
    vec3 velocity;
    vec3 scale;
    vec3 accel;
    vec4 color;
    float life;
};

layout (std430, binding = 0) readonly restrict buffer Particles {
    particle particles[];
};

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec2 inputTexCoord;
layout (location = 2) in vec3 inputNormal;
layout (location = 3) in vec3 inputTangent;

out vec4 Color;
out vec3 Normal;
out float Life;
out vec3 Position;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ObjectToWorldMatrix;


void main() {
    particle particleData = particles[gl_InstanceID];

    vec3 position = inputPosition * particleData.scale * clamp(particleData.life, 0, 1) + particleData.position;

    gl_Position = ProjectionMatrix * ViewMatrix * ObjectToWorldMatrix * vec4(position, 1.0);
    Color = particleData.color;
    Normal = (ObjectToWorldMatrix * vec4(inputNormal, 0.0)).xyz;
    Life = particleData.life;
    Position = (ObjectToWorldMatrix * vec4(position, 1.0)).xyz;
}