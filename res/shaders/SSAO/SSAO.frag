#version 460 core

in vec2 TexCoords;

uniform float Threshold;

layout (binding = 0) uniform sampler2D Normals;
layout (binding = 1) uniform sampler2D Noise;


out vec3 FragColor;

void main()
{
    vec3 normal = texture(Normals, TexCoords).rgb;
    vec3 randomVec = texture(Noise, TexCoords * noiseScale).xyz;

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * radius;

        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset;    // from view to clip-space
        offset.xyz /= offset.w;               // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5;
        float sampleDepth = texture(gPosition, offset.xy).z;
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0);

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;

        occlusion = 1.0 - (occlusion / kernelSize);
        FragColor = occlusion;
    }
}