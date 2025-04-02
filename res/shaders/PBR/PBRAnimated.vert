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

    float weightSum = 0.0;
for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
{
    if (boneIds[i] >= 0)
        weightSum += weights[i];
}

vec4 totalPosition = vec4(0.0f);
if (weightSum == 0.0) {
    totalPosition = vec4(inputPosition, 1.0);  // Use original position if no bones influence
}
if (weightSum > 0.0)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if (boneIds[i] == -1) continue;
        if (boneIds[i] >= MAX_BONES) break;

        float normalizedWeight = weights[i] / weightSum; // Normalize each weight
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(inputPosition, 1.0f);
        totalPosition += localPosition * normalizedWeight;
    }
}
else
{
    totalPosition = vec4(inputPosition, 1.0f); // Fallback to original position
}


    gl_Position = ProjectionMatrix * ViewMatrix * ObjectToWorldMatrix * totalPosition;


    Position = (ObjectToWorldMatrix * totalPosition).xyz;
    TexCoord = inputTexCoord;
    Normal = (ObjectToWorldMatrix * vec4(inputNormal, 0.0)).xyz;
    Tangent = (ObjectToWorldMatrix * vec4(inputTangent, 0.0)).xyz;
}
