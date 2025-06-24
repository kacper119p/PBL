#version 460 core
#extension GL_ARB_bindless_texture: enable

in vec2 UV;

layout (bindless_sampler) uniform sampler2D Sprite;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 OcclusionMask;

layout (early_fragment_tests) in;
void main()
{
    float alpha = texture(Sprite, UV).r;
    vec3 Color = vec3(63.43752, 4.76168, 0.74808) / 6;
    FragColor = vec4(Color, alpha);
    OcclusionMask = vec4(vec3(0.0), alpha);
}
