#version 460 core

uniform vec3 Tint;

in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;

out vec3 FragColor;

uniform sampler2D BaseMap;

void main()
{
    //FragColor = customColor * Tint * texture(BaseMap, TexCoord).rgb;
    FragColor = texture(BaseMap, TexCoord).rgb * max(dot(Normal, normalize(vec3(1.0f, 1.0f, 1.0f))), 0.0f);
}
