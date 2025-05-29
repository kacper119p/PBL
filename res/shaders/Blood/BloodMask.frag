#version 460 core

in vec2 TexCoords;

uniform vec3 Color;

layout (binding = 0) uniform sampler2D Image;

layout (location = 0) out vec4 FragColor;

void main()
{
    float alpha = texture(Image, TexCoords).a;
    alpha = step(0.33, alpha);
    FragColor = vec4(Color, alpha);
}