#version 460 core

in vec2 TexCoords;

uniform bool Horizontal;

layout (binding = 0) uniform sampler2D Image;

out float FragColor;

const float WEIGHTS[5] = float[](0.17954545454, 0.16704545454, 0.13295454545, 0.07954545454, 0.03068181818);
const int SAMPLES = 5;

void main()
{
    vec2 offset = 1.0 / textureSize(Image, 0);

    float result = texture(Image, TexCoords).r * WEIGHTS[0];

    if (Horizontal)
    {
        float aspectRatio = offset.x / offset.y;

        for (int i = 1; i < SAMPLES; ++i)
        {
            result += texture(Image, TexCoords + vec2(i * offset.x * aspectRatio, 0.0)).r * WEIGHTS[i];
            result += texture(Image, TexCoords - vec2(i * offset.x * aspectRatio, 0.0)).r * WEIGHTS[i];
        }
    }
    else
    {
        for (int i = 1; i < SAMPLES; ++i)
        {
            result += texture(Image, TexCoords + vec2(0.0, i * offset.y)).r * WEIGHTS[i];
            result += texture(Image, TexCoords - vec2(0.0, i * offset.y)).r * WEIGHTS[i];
        }
    }


    FragColor = result;
}