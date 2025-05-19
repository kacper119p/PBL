#version 460 core

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D Rays;
out vec3 FragColor;

void main() {
    FragColor = texture(Rays, TexCoords).rgb * 0.1;
}