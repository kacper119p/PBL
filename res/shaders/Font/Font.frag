#version 460 core

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D FontAtlas;

out vec4 FragColor;

void main() {
    FragColor = texture(FontAtlas, TexCoords);
}