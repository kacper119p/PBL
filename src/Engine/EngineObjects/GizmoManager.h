#pragma once

#if EDITOR
#include "imgui.h" //Have to import imgui before imguizmo
#include <ImGuizmo.h>
#include "Engine/Components/Transform.h"
#include "CameraRenderData.h"

namespace Engine
{
    /**
     * @brief Used for ImGuizmo operations.
     */
    class GizmoManager
    {
    private:
        ImGuizmo::OPERATION CurrentOperation = ImGuizmo::OPERATION::TRANSLATE;
        Transform* Managed = nullptr;

        static GizmoManager* Instance;

        float rectX, rectY = 0.0f;
        float rectWidth = 1920.0f;
        float rectHeight = 1080.0f;
    private:
        GizmoManager();

    public:
        [[nodiscard]] static GizmoManager* GetInstance()
        {
            return Instance;
        }

        [[nodiscard]] ImGuizmo::OPERATION GetCurrentOperation() const
        {
            return CurrentOperation;
        }

        void SetCurrentOperation(ImGuizmo::OPERATION CurrentOperation)
        {
            GizmoManager::CurrentOperation = CurrentOperation;
        }

        [[nodiscard]] Transform* GetManaged() const
        {
            return Managed;
        }

        void SetManaged(Transform* Managed)
        {
            GizmoManager::Managed = Managed;
        }

    public:
        static void Initialize();

        void Manipulate(const CameraRenderData& CameraRenderData);
        void SetRect(float x, float y, float width, float height)
        {
            rectX = x;
            rectY = y;
            rectWidth = width;
            rectHeight = height;
        }
    };

} // Engine
#endif
