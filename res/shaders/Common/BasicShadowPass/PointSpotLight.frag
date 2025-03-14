#version 460 core

in vec3 PositionWS;

uniform vec3 LightPosition;
uniform float Range;

void main()
{
    gl_FragDepth = length(PositionWS - LightPosition) / Range;
}