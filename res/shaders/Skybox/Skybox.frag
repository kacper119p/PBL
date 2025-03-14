#version 460 core

in vec3 Direction;

uniform samplerCube Texture;

out vec4 FragColor;

void main()
{
    vec3 color = texture(Texture, normalize(Direction)).rgb;
    FragColor = vec4(color, 1.0);
}