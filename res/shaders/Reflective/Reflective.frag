#version 460 core

in vec2 TexCoord;
in vec3 Position;
in vec3 Normal;
in vec3 Tangent;

uniform vec3 CameraPosition;
layout (binding = 0) uniform samplerCube EnvironmentMap;

out vec4 FragColor;

void main()
{
    vec3 I = normalize(Position - CameraPosition);
    vec3 R = reflect(I, normalize(Normal));

    vec3 Color = texture(EnvironmentMap, R).rgb;

    FragColor = vec4(Color, 1.0);
}
