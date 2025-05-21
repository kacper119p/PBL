#version 460 core

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D OcclusionMask;

layout (std430, binding = 0) buffer Lights {
    uint lightCount;
    vec2 lightPositions[];
};

out vec3 FragColor;

const float DECAY = 0.95;
const float DENSITY = 0.8;
const float WEIGHT = 0.4;
const float FALLOFF = 256.0;
const float EPSILON = 0.00001;
const uint MAX_SAMPLES = 64;

void main() {
    vec3 illumination = vec3(0.0);
    float totalWeight = 0.0;

    for (uint l = 0; l < lightCount; ++l)
    {
        vec2 offset = TexCoords - lightPositions[l];
        float distance2 = dot(offset, offset);
        float weight = 1.0 / (distance2 * FALLOFF + EPSILON);
        totalWeight += weight;

        vec2 deltaTexCoord = offset * DENSITY / float(MAX_SAMPLES);
        vec2 sampleCoords = TexCoords;
        float illuminationDecay = 1.0;

        vec3 accumulation = vec3(0.0f);
        for (uint i = 0; i < MAX_SAMPLES; ++i)
        {
            sampleCoords -= deltaTexCoord;

            vec3 sampled = texture(OcclusionMask, sampleCoords).rgb;
            accumulation += sampled * illuminationDecay;
            illuminationDecay *= DECAY;
        }
        illumination += accumulation * weight;
    }

    FragColor = illumination * (WEIGHT / max(totalWeight, EPSILON));
}
