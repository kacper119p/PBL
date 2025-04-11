#pragma once

#include "Engine/Components/Transform.h"
namespace Engine
{
    class SceneHierarchyGUI
    {
    private:
        Transform* Root;
        Transform* SelectedEntity;
    public:
        SceneHierarchyGUI() = default;
        SceneHierarchyGUI(Transform* Root);
        void SetRoot(Transform* root)
        {
            Root = root;
            SelectedEntity = root;
        }
        void DrawHierarchy(Transform* entity);
        void Draw();
    };
} // namespace Engine
