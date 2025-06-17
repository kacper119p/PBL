#version 460 core
#extension GL_ARB_bindless_texture: enable

in vec2 TexCoord;

uniform float Time;

layout (bindless_sampler) uniform sampler2D Texture;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 OcclusionMask;

layout (early_fragment_tests) in;
void main()
{
    float alpha = texture(Texture, TexCoord + vec2(Time, -Time)).r;
    FragColor = vec4(vec3(1.0), alpha);
    OcclusionMask = vec4(vec3(0.0), alpha);
}
