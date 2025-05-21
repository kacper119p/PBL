#version 460 core

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D Rays;
out vec3 FragColor;

const float EXPOSURE = 0.1;

void main() {
    FragColor = texture(Rays, TexCoords).rgb * EXPOSURE;
}