#version 460 core

#include "FlameParticleData.glsl"

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

    // Extract camera right and up from view matrix (assuming row-major and view matrix is world->camera)

    vec3 cameraRight = vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
    vec3 cameraUp = vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

    // Particle world position
    vec3 worldPosition = (ObjectToWorldMatrix * vec4(particleData.position, 1.0)).xyz;

    // Calculate billboard vertex position
    vec3 billboardOffset = ((inputTexCoord.x - 0.5) * cameraRight + (inputTexCoord.y - 0.5) * cameraUp) * particleData.scale;

    vec3 finalPos = worldPosition + billboardOffset;

    gl_Position = ProjectionMatrix * ViewMatrix * vec4(finalPos, 1.0);

    // Sprite animation frame calculation
    int frameIndex = min(int(floor((MAX_LIFETIME - particleData.life) / FRAME_DURATION)), SPRITE_ROWS * SPRITE_COLUMNS - 1);
    frameIndex = frameIndex % (SPRITE_COLUMNS * SPRITE_ROWS);

    int frameX = frameIndex % SPRITE_COLUMNS;
    int frameY = frameIndex / SPRITE_COLUMNS;

    // Flip Y if necessary depending on your sprite sheet layout:
    frameY = (SPRITE_ROWS - 1) - frameY;

    vec2 frameOffset = vec2(frameX, frameY) / vec2(SPRITE_COLUMNS, SPRITE_ROWS);
    vec2 frameScale = vec2(1.0 / SPRITE_COLUMNS, 1.0 / SPRITE_ROWS);

    UV = inputTexCoord * frameScale + frameOffset;
}
