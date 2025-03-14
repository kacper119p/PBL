#version 460 core

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D Image;

out vec4 FragColor;

const float WEIGHTS[10] = float[](0.1470, 0.1373, 0.1121, 0.0798, 0.0497, 0.0270, 0.0128, 0.0053, 0.0019, 0.0006);
const int SAMPLES = 10;
const float SIZE = 0.001;

void main()
{
    vec2 texOffset = 1.0 / textureSize(Image, 0);

    vec4 result = texture(Image, TexCoords) * WEIGHTS[0] * WEIGHTS[0];

    for (int i = 1; i < SAMPLES; ++i)
    {
        result += texture(Image, TexCoords + vec2(0.0, i * SIZE)) * WEIGHTS[i];
        result += texture(Image, TexCoords - vec2(0.0, i * SIZE)) * WEIGHTS[i];
    }

    FragColor = result;
}