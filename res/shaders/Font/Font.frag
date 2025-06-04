#version 460 core

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D FontAtlas;

uniform vec4 Color;

out vec4 FragColor;

float Median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

void main()
{
    vec3 s = texture(FontAtlas, TexCoords).rgb;
    float d = Median(s.r, s.g, s.b) - 0.5;
    float a = clamp(d / fwidth(d) + 0.5, 0.0, 1.0);
    FragColor = vec4(Color.rgb, a * Color.a);
}