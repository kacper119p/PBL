#include "imgui.h"
#include "TransformGui.h"

namespace Engine
{
    TransformGui::~TransformGui()
    {

    }

    void TransformGui::Draw(Transform* Transform)
    {
        glm::vec3 tmp = Transform->GetPosition();
        float position[3]{tmp.x, tmp.y, tmp.z};
        tmp = Transform->GetEulerAngles();
        float rotation[3]{tmp.x, tmp.y, tmp.z};
        tmp = Transform->GetScale();
        float scale[3]{tmp.x, tmp.y, tmp.z};

        if (ImGui::InputFloat3("Position", position))
        {
            Transform->SetPosition(glm::vec3(position[0], position[1], position[2]));
        }
        if (ImGui::InputFloat3("Rotation", rotation))
        {
            Transform->SetEulerAngles(glm::vec3(rotation[0], rotation[1], rotation[2]));
        }
        if (ImGui::InputFloat3("Scale", scale))
        {
            Transform->SetScale(glm::vec3(scale[0], scale[1], scale[2]));
        }
    }
} // Engine
