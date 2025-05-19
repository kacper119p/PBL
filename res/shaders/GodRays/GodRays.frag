#version 460 core

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D uLightMask;

layout (std430, binding = 0) buffer Lights {
    float lightCount;
    vec2 lightPositions[];
};

out vec3 FragColor;

const float DECAY = 0.95;
const float DENSITY = 0.8;
const float WEIGHT = 0.4;
const int NUM_SAMPLES = 100;


void main() {
    vec3 illumination = vec3(0.0);
    float weight = WEIGHT / lightCount;

    for (int l = 0; l < lightCount; ++l) {
        vec2 lightPos = lightPositions[l];
        vec2 deltaTexCoord = (TexCoords - lightPos) * (DENSITY / float(NUM_SAMPLES));

        vec2 coord = TexCoords;
        float decay = 1.0;

        for (int i = 0; i < NUM_SAMPLES; ++i)
        {
            coord -= deltaTexCoord;
            if (coord.x < 0.0 || coord.y < 0.0 || coord.x > 1.0 || coord.y > 1.0)
            {
                break;
            }
            vec3 sampled = texture(uLightMask, coord).rgb;
            illumination += sampled * decay * weight;
            decay *= DECAY;
        }
    }

    FragColor = illumination;
}