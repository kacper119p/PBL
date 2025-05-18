#version 460 core

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D SceneColor;
layout (binding = 1) uniform sampler2D BloomColor;

const float GAMMA = 2.2;
const float EXPOSURE = 0.25;
const float INTENSITY = 0.1;

out vec3 FragColor;

void main()
{
    vec3 hdrColor = texture(SceneColor, TexCoords).rgb;
    vec3 bloomColor = max(vec3(0.0), texture(BloomColor, TexCoords).rgb) * INTENSITY;

    hdrColor += bloomColor;

    vec3 result = vec3(1.0) - exp(-hdrColor * EXPOSURE);

    result = pow(result, vec3(1.0 / GAMMA));

    FragColor = result;
}