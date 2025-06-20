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
            return;

        glm::vec3 center(0.0f);
        for (auto* managed : this->Managed)
        {
            if (managed)
                center += managed->GetPosition();
        }
        center /= static_cast<float>(this->Managed.size());

        float transformMatrix[16];

        glm::vec3 zeroRotation(0.0f);
        glm::vec3 oneScale(1.0f);

        ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(center),
                                                glm::value_ptr(zeroRotation),
                                                glm::value_ptr(oneScale),
                                                transformMatrix);

        ImGuizmo::SetOrthographic(true);
        const float* viewMatrixPtr = glm::value_ptr(CameraRenderData.ViewMatrix);
        const float* projectionMatrixPtr = glm::value_ptr(CameraRenderData.ProjectionMatrix);

        const ImGuizmo::MODE mode = (CurrentOperation == ImGuizmo::OPERATION::ROTATE)
                                        ? ImGuizmo::MODE::LOCAL
                                        : ImGuizmo::MODE::WORLD;

        float deltaMatrix[16];
        ImGuizmo::Manipulate(viewMatrixPtr,
                             projectionMatrixPtr,
                             CurrentOperation,
                             mode,
                             transformMatrix,
                             deltaMatrix);

        float translation[3];
        float rotation[3];
        float scale[3];
        ImGuizmo::DecomposeMatrixToComponents(deltaMatrix, translation, rotation, scale);

        for (Transform* managed : this->Managed)
        {
            if (!managed)
                continue;

            switch (CurrentOperation)
            {
                case ImGuizmo::OPERATION::TRANSLATE:
                {
                    managed->SetPosition(
                            managed->GetPosition() + glm::vec3(translation[0], translation[1], translation[2]));
                    break;
                }
                case ImGuizmo::OPERATION::ROTATE:
                {
                    managed->SetEulerAngles(
                            managed->GetEulerAngles() + glm::vec3(rotation[0], rotation[1], rotation[2]));
                    break;
                }
                case ImGuizmo::OPERATION::SCALE:
                {
                    glm::vec3 currentScale = managed->GetScale();
                    managed->SetScale(glm::vec3(currentScale.x * scale[0], currentScale.y * scale[1],
                                                currentScale.z * scale[2]));
                    break;
                }
            }
        }
    }
} // Engine
#endif
