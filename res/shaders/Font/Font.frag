#version 460 core

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D FontAtlas;

out vec4 FragColor;

const float PX_RANGE = 2.0;

float Median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

float ScreenPxRange()
{
    vec2 unitRange = vec2(PX_RANGE) / vec2(textureSize(FontAtlas, 0));
    vec2 screenTexSize = vec2(1.0) / fwidth(TexCoords);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

void main()
{
    vec3 msd = texture(FontAtlas, TexCoords).rgb;
    float sd = Median(msd.r, msd.g, msd.b);
    float screenPxDistance = ScreenPxRange() * (sd - 0.5);
    float opacity = smoothstep(-1, 1, screenPxDistance + 0.5);
    FragColor = vec4(vec3(0), opacity);
}