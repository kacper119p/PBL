#version 460 core
#extension GL_ARB_bindless_texture: enable

in vec2 TexCoord;
in vec3 Position;
in vec3 Normal;
in vec3 Tangent;

uniform vec3 CameraPosition;
layout (bindless_sampler) uniform samplerCube EnvironmentMap;

out vec4 FragColor;

layout (early_fragment_tests) in;
void main()
{
    vec3 I = normalize(Position - CameraPosition);
    vec3 R = reflect(I, normalize(Normal));

    vec3 Color = texture(EnvironmentMap, R).rgb;

    FragColor = vec4(Color, 1.0);
}
