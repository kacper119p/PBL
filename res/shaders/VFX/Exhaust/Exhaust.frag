#version 460 core
#extension GL_ARB_bindless_texture: enable

in vec2 UV;
in float Lifetime;

layout (bindless_sampler) uniform sampler2D Sprite;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 OcclusionMask;

const float ALPHA_MULTIPLIER = 0.25;

layout (early_fragment_tests) in;
void main()
{
    float alpha = texture(Sprite, UV).r;
    FragColor = vec4(vec3(0.5), clamp(Lifetime, 0.0, 1.0) * ALPHA_MULTIPLIER * alpha);
    OcclusionMask = vec4(vec3(0.0), alpha * ALPHA_MULTIPLIER);
}
