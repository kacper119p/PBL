#version 460 core

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D InputTexture;
layout (location = 0) out vec3 FragColor;

void main()
{
    vec2 srcTexelSize = 1.0 / vec2(textureSize(InputTexture, 0));
    float x = srcTexelSize.x;
    float y = srcTexelSize.y;

    vec3 a = texture(InputTexture, vec2(TexCoords.x - 2 * x, TexCoords.y + 2 * y)).rgb;
    vec3 b = texture(InputTexture, vec2(TexCoords.x, TexCoords.y + 2 * y)).rgb;
    vec3 c = texture(InputTexture, vec2(TexCoords.x + 2 * x, TexCoords.y + 2 * y)).rgb;

    vec3 d = texture(InputTexture, vec2(TexCoords.x - 2 * x, TexCoords.y)).rgb;
    vec3 e = texture(InputTexture, vec2(TexCoords.x, TexCoords.y)).rgb;
    vec3 f = texture(InputTexture, vec2(TexCoords.x + 2 * x, TexCoords.y)).rgb;

    vec3 g = texture(InputTexture, vec2(TexCoords.x - 2 * x, TexCoords.y - 2 * y)).rgb;
    vec3 h = texture(InputTexture, vec2(TexCoords.x, TexCoords.y - 2 * y)).rgb;
    vec3 i = texture(InputTexture, vec2(TexCoords.x + 2 * x, TexCoords.y - 2 * y)).rgb;

    vec3 j = texture(InputTexture, vec2(TexCoords.x - x, TexCoords.y + y)).rgb;
    vec3 k = texture(InputTexture, vec2(TexCoords.x + x, TexCoords.y + y)).rgb;
    vec3 l = texture(InputTexture, vec2(TexCoords.x - x, TexCoords.y - y)).rgb;
    vec3 m = texture(InputTexture, vec2(TexCoords.x + x, TexCoords.y - y)).rgb;

    FragColor = e * 0.125;
    FragColor += (a + c + g + i) * 0.03125;
    FragColor += (b + d + f + h) * 0.0625;
    FragColor += (j + k + l + m) * 0.125;
    FragColor = max(FragColor, 0.0001f);
}