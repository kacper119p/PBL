#version 460 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec2 inputTexCoord;
layout (location = 2) in vec3 inputNormal;
layout (location = 3) in vec3 inputTangent;

out vec2 UV;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ObjectToWorldMatrix;

void main()
{
    vec3 camForward = normalize(vec3(ViewMatrix[0][2], 0.0, ViewMatrix[2][2]));
    vec3 cameraRight = normalize(cross(vec3(0.0, 1.0, 0.0), camForward));
    vec3 cameraUp = vec3(0.0, 1.0, 0.0);

    vec3 offset = (inputTexCoord.x - 0.5) * cameraRight + (inputTexCoord.y - 0.5) * cameraUp;
    vec3 basePosition = vec3(ObjectToWorldMatrix[3]);
    vec3 worldPosition = basePosition + offset;

    gl_Position = ProjectionMatrix * ViewMatrix * vec4(worldPosition, 1.0);

    UV = inputTexCoord;
}
