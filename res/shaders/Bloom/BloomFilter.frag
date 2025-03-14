#version 460 core

in vec2 TexCoords;

uniform float Threshold;

layout (binding = 0) uniform sampler2D Image;

out vec3 FragColor;

void main() {
    vec3 result = texture(Image, TexCoords).rgb;
    FragColor = result * step(Threshold, dot(result, vec3(0.2126, 0.7152, 0.0722)));
}