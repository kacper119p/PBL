#if EDITOR
#include "GizmoManager.h"
#include "glm/gtc/type_ptr.hpp"

namespace Engine
{
    GizmoManager* GizmoManager::Instance = nullptr;

    GizmoManager::GizmoManager()
    = default;

    void GizmoManager::Initialize()
    {
        Instance = new GizmoManager();
    }

    void GizmoManager::Manipulate(const CameraRenderData& CameraRenderData)
    {
        if (this->Managed.empty())
        {
            return;
        }

        Transform* currentManaged = this->Managed.back();

        if (currentManaged == nullptr)
        {
            return;
        }

        float transformMatrix[16];

        ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(currentManaged->GetPosition()),
                                                glm::value_ptr(currentManaged->GetEulerAngles()),
                                                glm::value_ptr(currentManaged->GetScale()),
                                                transformMatrix);

        //ImGuizmo::SetRect(rectX, rectY, rectWidth, rectHeight);
        ImGuizmo::SetOrthographic(true);
        const float* viewMatrixPtr = glm::value_ptr(CameraRenderData.ViewMatrix);
        const float* projectionMatrixPtr = glm::value_ptr(CameraRenderData.ProjectionMatrix);
        const ImGuiIO& io = ImGui::GetIO();


        const ImGuizmo::MODE mode = (CurrentOperation == ImGuizmo::OPERATION::ROTATE)
                                        ? ImGuizmo::MODE::LOCAL
                                        : ImGuizmo::MODE::WORLD;
        float deltaMatrix[16];
        ImGuizmo::Manipulate(viewMatrixPtr,
                             projectionMatrixPtr, CurrentOperation,
                             mode, transformMatrix, deltaMatrix);

        float refMatrix[16] = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        };

        for (int i = 0; i < 16; i++)
        {
            if (deltaMatrix[i] != refMatrix[i])
            {
                int dupa = 3;
            }
        }


        float translation[3];
        float rotation[3];
        float scale[3];
        ImGuizmo::DecomposeMatrixToComponents(deltaMatrix, translation, rotation, scale);

        for (Transform* managed : this->Managed)
        {
            switch (CurrentOperation)
            {
                case ImGuizmo::OPERATION::TRANSLATE:
                    managed->SetPosition(managed->GetPosition() +
                                         glm::vec3(translation[0], translation[1], translation[2]));
                    break;
                case ImGuizmo::OPERATION::ROTATE:
                    managed->SetEulerAngles(managed->GetEulerAngles() +
                                            glm::vec3(rotation[0], rotation[1], rotation[2]));
                    break;
                case ImGuizmo::OPERATION::SCALE:
                    managed->SetScale(glm::vec3(managed->GetScale().x * scale[0], managed->GetScale().y * scale[1],
                                                managed->GetScale().z * scale[2]));
                    break;

            }
        }
    }
} // Engine
#endif
