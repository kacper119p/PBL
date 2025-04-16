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
    };

} // Engine
#endif
