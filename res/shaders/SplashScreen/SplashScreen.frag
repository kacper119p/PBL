#version 460 core
#extension GL_ARB_bindless_texture: enable

in vec2 TexCoords;

uniform float Alpha;

layout (bindless_sampler) uniform sampler2D Image;

layout (location = 0) out vec4 FragColor;

void main()
{
    vec4 color = texture(Image, TexCoords);
    FragColor = vec4(color.rgb, color.a * Alpha);
}