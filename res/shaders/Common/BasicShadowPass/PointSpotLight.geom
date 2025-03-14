#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 ShadowMatrices[6];

out vec3 PositionWS;

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        gl_Layer = face;
        for (int i = 0; i < 3; ++i)
        {
            PositionWS = vec3(gl_in[i].gl_Position);
            gl_Position = ShadowMatrices[face] * gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}