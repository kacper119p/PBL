#version 460 core

uniform mat4 ObjectToWorldMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

in vec4 NormalDepth;

layout (location = 1) out vec4 OutNormalDepth;

void main()
{
    OutNormalDepth.xyz = normalize(NormalDepth.xyz);
    OutNormalDepth.w = NormalDepth.w;
}
