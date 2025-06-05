#version 460 core

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D Image;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 OcclusionMask;

void main()
{
    vec4 textureInput = texture(Image, TexCoords);
    vec3 color = pow(textureInput.rgb, vec3(2.2)); // To linear space
    FragColor = vec4(color, textureInput.a);
    OcclusionMask = vec4(vec3(0.0), textureInput.a);
}