#version 460 core
#extension GL_ARB_bindless_texture: enable

in vec3 Direction;

layout (binding = 0) uniform samplerCube Texture;

layout (location = 0) out vec3 FragColor;
layout (location = 1) out vec3 OcclusionMask;

layout (early_fragment_tests) in;
void main()
{
    vec3 color = texture(Texture, normalize(Direction)).rgb;
    FragColor = color;
    float luminance = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    OcclusionMask = FragColor * step(5.0f, luminance);
}