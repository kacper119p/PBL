#version 460 core

in vec2 TexCoords;

uniform float Alpha;

layout (binding = 0) uniform sampler2D Image;

layout (location = 0) out vec4 FragColor;

void main()
{
    vec4 color = texture(Image, TexCoords);
    FragColor = vec4(color.rgb, color.a * Alpha);
}