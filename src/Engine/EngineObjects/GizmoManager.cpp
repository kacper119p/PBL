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
        if (Managed == nullptr)
        {
            return;
        }
        float transformMatrix[16];

        ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(Managed->GetPosition()),
                                                glm::value_ptr(Managed->GetEulerAngles()),
                                                glm::value_ptr(Managed->GetScale()), transformMatrix);

        const float* viewMatrixPtr = glm::value_ptr(CameraRenderData.ViewMatrix);
        const float* projectionMatrixPtr = glm::value_ptr(CameraRenderData.ProjectionMatrix);

        // Use custom rect for the gizmo
        ImGuizmo::SetRect(rectX, rectY, rectWidth, rectHeight);

        const ImGuizmo::MODE mode =
                (CurrentOperation == ImGuizmo::OPERATION::ROTATE) ? ImGuizmo::MODE::LOCAL : ImGuizmo::MODE::WORLD;

        ImGuizmo::Manipulate(viewMatrixPtr, projectionMatrixPtr, CurrentOperation, mode, transformMatrix);

        if (!ImGuizmo::IsUsing())
        {
            return;
        }

        float translation[3];
        float rotation[3];
        float scale[3];
        ImGuizmo::DecomposeMatrixToComponents(transformMatrix, translation, rotation, scale);

        Managed->SetPosition(glm::vec3(translation[0], translation[1], translation[2]));
        Managed->SetEulerAngles(glm::vec3(rotation[0], rotation[1], rotation[2]));
        Managed->SetScale(glm::vec3(scale[0], scale[1], scale[2]));
    }

    void GizmoManager::SetRects(float rectX, float rectY, float rectWidth, float rectHeight) 
    { rectX = rectX;
        rectY = rectY;
        rectWidth = rectWidth;
        rectHeight = rectHeight;
        ImGuizmo::SetRect(rectX, rectY, rectWidth, rectHeight);
    }

} // Engine
#endif
