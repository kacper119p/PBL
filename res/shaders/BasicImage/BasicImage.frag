#version 460 core

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D Image;

out vec4 FragColor;

void main()
{
    vec4 textureInput = texture(Image, TexCoords);
    vec3 color = pow(textureInput.rgb, vec3(2.2)); // To linear space
    FragColor = vec4(color, textureInput.a);
}