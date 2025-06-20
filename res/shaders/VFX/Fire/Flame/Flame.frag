#version 460 core
#extension GL_ARB_bindless_texture: enable

in vec2 UV;

layout (bindless_sampler) uniform sampler2D SpriteSheet;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 OcclusionMask;

layout (early_fragment_tests) in;
void main()
{
    vec4 Color = vec4(texture(SpriteSheet, UV).rgb, 1.0);
    FragColor = Color;
    OcclusionMask = Color * 0.5;
}
