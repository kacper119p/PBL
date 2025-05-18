#version 460 core

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D InputTexture;

in vec2 TexCoords;
layout (location = 0) out vec3 FragColor;

const float FILTER_RADIUS = 0.5;

void main()
{
    float x = FILTER_RADIUS;
    float y = FILTER_RADIUS;

    vec3 a = texture(InputTexture, vec2(TexCoords.x - x, TexCoords.y + y)).rgb;
    vec3 b = texture(InputTexture, vec2(TexCoords.x, TexCoords.y + y)).rgb;
    vec3 c = texture(InputTexture, vec2(TexCoords.x + x, TexCoords.y + y)).rgb;

    vec3 d = texture(InputTexture, vec2(TexCoords.x - x, TexCoords.y)).rgb;
    vec3 e = texture(InputTexture, vec2(TexCoords.x, TexCoords.y)).rgb;
    vec3 f = texture(InputTexture, vec2(TexCoords.x + x, TexCoords.y)).rgb;

    vec3 g = texture(InputTexture, vec2(TexCoords.x - x, TexCoords.y - y)).rgb;
    vec3 h = texture(InputTexture, vec2(TexCoords.x, TexCoords.y - y)).rgb;
    vec3 i = texture(InputTexture, vec2(TexCoords.x + x, TexCoords.y - y)).rgb;


    FragColor = e * 4.0;
    FragColor += (b + d + f + h) * 2.0;
    FragColor += (a + c + g + i);
    FragColor *= 1.0 / 16.0;
}