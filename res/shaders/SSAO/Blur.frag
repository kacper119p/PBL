#version 460 core

in vec2 TexCoords;

uniform bool Horizontal;

layout (binding = 0) uniform sampler2D Image;

out float FragColor;

const int SAMPLES = 4;

void main()
{
    vec2 offset = 1.0 / textureSize(Image, 0);

    float result = texture(Image, TexCoords).r;

    if (Horizontal)
    {
        for (int i = 0; i < SAMPLES; ++i)
        {
            result += texture(Image, TexCoords + vec2(i * offset.x, 0.0)).r;
            result += texture(Image, TexCoords - vec2(i * offset.x, 0.0)).r;
        }
    }
    else
    {
        for (int i = 0; i < SAMPLES; ++i)
        {
            result += texture(Image, TexCoords + vec2(0.0, i * offset.y)).r;
            result += texture(Image, TexCoords - vec2(0.0, i * offset.y)).r;
        }
    }

    FragColor = result / (SAMPLES * 2 + 1);
}