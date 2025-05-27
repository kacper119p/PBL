#version 460 core

in vec2 TexCoords;

uniform vec3 Color;

layout (binding = 0) uniform sampler2D Image;

layout (location = 0) out vec4 FragColor;

void main()
{
    vec4 Result = texture(Image, TexCoords);
    Result.rgb *= Color;
    FragColor = Result;
}