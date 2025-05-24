#version 460 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec2 inputTexCoord;
layout (location = 2) in vec3 inputNormal;
layout (location = 3) in vec3 inputTangent;
layout (location = 4) in ivec4 boneIds;
layout (location = 5) in vec4 weights;

out vec2 TexCoord;
out vec3 Position;
out vec3 Normal;
out vec3 Tangent;

uniform mat4 ObjectToWorldMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
    vec4 totalPosition = vec4(0.0);
    vec3 totalNormal = vec3(0.0);
    vec3 totalTangent = vec3(0.0);

    float totalWeight = 0.0;

    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        int boneId = boneIds[i];
        float weight = weights[i];

        if (boneId < 0 || boneId >= MAX_BONES || weight == 0.0)
        continue;

        totalWeight += weight;

        mat4 boneMatrix = finalBonesMatrices[boneId];
        mat3 boneMatrix3 = mat3(boneMatrix); // Assumes no non-uniform scale

        totalPosition += boneMatrix * vec4(inputPosition, 1.0) * weight;
        totalNormal += boneMatrix3 * inputNormal * weight;
        totalTangent += boneMatrix3 * inputTangent * weight;
    }

    if (totalWeight == 0.0)
    {
        totalPosition = vec4(inputPosition, 1.0);
        totalNormal = inputNormal;
        totalTangent = inputTangent;
    }

    mat4 modelMatrix = ObjectToWorldMatrix;
    mat4 viewProjMatrix = ProjectionMatrix * ViewMatrix;

    gl_Position = viewProjMatrix * modelMatrix * totalPosition;

    Position = (modelMatrix * totalPosition).xyz;
    TexCoord = inputTexCoord;

    Normal = normalize(mat3(modelMatrix) * totalNormal);
    Tangent = normalize(mat3(modelMatrix) * totalTangent);
}
