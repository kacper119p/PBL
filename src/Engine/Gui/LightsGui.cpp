#include <format>
#include "LightsGui.h"
#include "imgui.h"
#include "TransformGui.h"
#include "Engine/EngineObjects/GizmoManager.h"

namespace Engine
{
    std::vector<DirectionalLight*> LightsGui::DirectionalLights = std::vector<DirectionalLight*>();
    std::vector<PointLight*> LightsGui::PointLights = std::vector<PointLight*>();
    std::vector<SpotLight*> LightsGui::SpotLights = std::vector<SpotLight*>();
    Component* LightsGui::Current = nullptr;
    std::string LightsGui::CurrentName = std::string();

    LightsGui::~LightsGui() = default;

    void LightsGui::Draw()
    {
        for (int i = 0; i < DirectionalLights.size(); ++i)
        {
            std::string name = std::format("Directional Light {}", i + 1);
            if (ImGui::Button(name.c_str()))
            {
                if (Current == DirectionalLights[i])
                {
                    GizmoManager::GetInstance()->SetManaged(nullptr);
                    Current = nullptr;
                    CurrentName = std::string();
                }
                else
                {
                    Current = DirectionalLights[i];
                    GizmoManager::GetInstance()->SetManaged(DirectionalLights[i]->GetOwner()->GetTransform());
                    CurrentName = name;
                }
            }
        }
        for (int i = 0; i < PointLights.size(); ++i)
        {
            std::string name = std::format("Point Light {}", i + 1);
            if (ImGui::Button(name.c_str()))
            {
                if (Current == PointLights[i])
                {
                    GizmoManager::GetInstance()->SetManaged(nullptr);
                    Current = nullptr;
                    CurrentName = std::string();
                }
                else
                {
                    Current = PointLights[i];
                    GizmoManager::GetInstance()->SetManaged(PointLights[i]->GetOwner()->GetTransform());
                    CurrentName = name;
                }
            }
        }
        for (int i = 0; i < SpotLights.size(); ++i)
        {
            std::string name = std::format("Spot Light {}", i + 1);
            if (ImGui::Button(name.c_str()))
            {
                if (Current == SpotLights[i])
                {
                    GizmoManager::GetInstance()->SetManaged(nullptr);
                    Current = nullptr;
                    CurrentName = std::string();
                }
                else
                {
                    Current = SpotLights[i];
                    GizmoManager::GetInstance()->SetManaged(SpotLights[i]->GetOwner()->GetTransform());
                    CurrentName = name;
                }
            }
        }

        if (Current == nullptr)
        {
            return;
        }
        ImGui::Text("%s", CurrentName.c_str());
        if (DirectionalLight* directionalLight = dynamic_cast<DirectionalLight*>(Current))
        {
            LightGui(directionalLight);
            TransformGui::Draw(directionalLight->GetOwner()->GetTransform());
        }
        else if (PointLight* pointLight = dynamic_cast<PointLight*>(Current))
        {
            LightGui(pointLight);
            TransformGui::Draw(pointLight->GetOwner()->GetTransform());
        }
        else if (SpotLight* spotLight = dynamic_cast<SpotLight*>(Current))
        {
            LightGui(spotLight);
            TransformGui::Draw(spotLight->GetOwner()->GetTransform());
        }
    }

    void LightsGui::LightGui(DirectionalLight* Light)
    {
        glm::vec3 color = Light->GetColor();
        if (ManageColor(color))
        {
            Light->SetColor(color);
        }
    }

    void LightsGui::LightGui(PointLight* Light)
    {
        glm::vec3 color = Light->GetColor();
        if (ManageColor(color))
        {
            Light->SetColor(color);
        }

        float value = Light->GetLinearFalloff();
        if (ImGui::InputFloat("LinearFalloff", &value))
        {
            Light->SetLinearFalloff(value);
        }
        value = Light->GetQuadraticFalloff();
        if (ImGui::InputFloat("QuadraticFalloff", &value))
        {
            Light->SetQuadraticFalloff(value);
        }
        value = Light->GetRange();
        if (ImGui::InputFloat("Range", &value))
        {
            Light->SetRange(value);
        }
    }

    void LightsGui::LightGui(SpotLight* Light)
    {
        glm::vec3 color = Light->GetColor();
        if (ManageColor(color))
        {
            Light->SetColor(color);
        }

        float value = Light->GetLinearFalloff();
        if (ImGui::InputFloat("LinearFalloff", &value))
        {
            Light->SetLinearFalloff(value);
        }
        value = Light->GetQuadraticFalloff();
        if (ImGui::InputFloat("QuadraticFalloff", &value))
        {
            Light->SetQuadraticFalloff(value);
        }
        value = Light->GetRange();
        if (ImGui::InputFloat("Range", &value))
        {
            Light->SetRange(value);
        }

        value = Light->GetOuterAngle();
        if (ImGui::InputFloat("OuterAngle", &value))
        {
            Light->SetOuterAngle(value);
        }

        value = Light->GetInnerAngle();
        if (ImGui::InputFloat("InnerAngle", &value))
        {
            Light->SetInnerAngle(value);
        }
    }

    bool LightsGui::ManageColor(glm::vec3& Color)
    {
        float color[3] = {Color.x, Color.y, Color.z};
        if (ImGui::InputFloat3("Color", color))
        {
            Color = glm::vec3(color[0], color[1], color[2]);
            return true;
        }
        return false;
    }
} // Engine
