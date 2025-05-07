#version 460 core
#extension GL_ARB_bindless_texture: enable

in vec3 Direction;

layout (bindless_sampler) uniform samplerCube Texture;

out vec4 FragColor;

layout (early_fragment_tests) in;
void main()
{
    vec3 color = texture(Texture, normalize(Direction)).rgb;
    FragColor = vec4(color, 1.0);
}