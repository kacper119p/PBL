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
out float Lifetime;

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

    // SpriteSheet Animation
    int frameIndex = min(int(floor((MAX_LIFETIME - particleData.life) / FRAME_DURATION)), SPRITE_ROWS * SPRITE_COLUMNS - 1);
    frameIndex = frameIndex % (SPRITE_COLUMNS * SPRITE_ROWS);

    int frameX = frameIndex % SPRITE_COLUMNS;
    int frameY = frameIndex / SPRITE_COLUMNS;

    frameY = (SPRITE_ROWS - 1) - frameY;

    vec2 frameOffset = vec2(frameX, frameY) / vec2(SPRITE_COLUMNS, SPRITE_ROWS);
    vec2 frameScale = vec2(1.0 / SPRITE_COLUMNS, 1.0 / SPRITE_ROWS);

    UV = inputTexCoord * frameScale + frameOffset;
    Lifetime = particleData.life;
}
