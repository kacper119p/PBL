#version 460 core

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D Image;

out layout (location = 0) vec3 FragColor;

const float THRESHOLD = 1.0;

void main()
{
    vec3 color = texture(Image, TexCoords).rgb;
    float brightness = max(color.r, max(color.g, color.b));
    float contribution = max(0, brightness - THRESHOLD);
    contribution /= max(brightness, 0.00001);
    FragColor = color * contribution;
}