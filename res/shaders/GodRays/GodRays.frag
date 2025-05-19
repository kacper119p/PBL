#version 460 core

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D OcclusionMask;

layout (std430, binding = 0) buffer Lights {
    float lightCount;
    vec2 lightPositions[];
};

out vec3 FragColor;

const float DECAY = 0.95;
const float DENSITY = 0.8;
const float WEIGHT = 0.4;
const float FALLOFF = 128.0;
const float EPSILON = 0.00001;
const int MAX_SAMPLES = 100;

void main() {
    vec3 illumination = vec3(0.0);
    float totalWeight = 0.0;

    for (int l = 0; l < int(lightCount); ++l)
    {
        vec2 offset = TexCoords - lightPositions[l];
        float dist2 = dot(offset, offset);
        float weight = 1.0 / (dist2 * FALLOFF + EPSILON);
        totalWeight += weight;

        vec2 deltaTexCoord = offset * DENSITY / float(MAX_SAMPLES);
        vec2 sampleCoords = TexCoords;
        float illuminationDecay = 1.0;

        for (int i = 0; i < MAX_SAMPLES; ++i) {
            sampleCoords -= deltaTexCoord;
            if (sampleCoords.x < 0.0 || sampleCoords.y < 0.0 || sampleCoords.x > 1.0 || sampleCoords.y > 1.0)
            {
                break;
            }

            vec3 sampled = texture(OcclusionMask, sampleCoords).rgb;
            illumination += sampled * illuminationDecay * weight;
            illuminationDecay *= DECAY;
        }
    }

    FragColor = illumination * (WEIGHT / max(totalWeight, EPSILON));
}
