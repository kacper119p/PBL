#version 460 core

in vec2 TexCoord;
in vec3 Position;
in vec3 Normal;
in vec3 Tangent;

uniform vec3 CameraPosition;
uniform float IOR;
layout (binding = 0) uniform samplerCube EnvironmentMap;

out vec4 FragColor;

void main()
{
    float ratio = 1.0 / IOR;

    vec3 I = normalize(Position - CameraPosition);
    vec3 R = refract(I, normalize(Normal), ratio);

    vec3 Color = texture(EnvironmentMap, R).rgb;

    FragColor = vec4(Color, 1.0);
}
