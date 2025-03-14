#version 460 core

in vec3 LocalPosition;

uniform sampler2D Texture;

out vec4 FragColor;

const vec2 INV_ATAN = vec2(0.1591, 0.3183);

void main()
{
    vec3 Direction = normalize(LocalPosition);
    vec2 uv = vec2(atan(Direction.z, Direction.x), asin(Direction.y));
    uv *= INV_ATAN;
    uv += 0.5;
    vec3 color = texture(Texture, uv).rgb;
    FragColor =vec4 (color, 1);
}