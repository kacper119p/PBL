#pragma once
#if EDITOR
#include "Engine/Components/Transform.h"
#include "Engine/EngineObjects/Entity.h"

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
            SelectedEntity = nullptr;
        }
        void DrawHierarchy(Transform* entity, Scene* scene);
        void Draw(Scene* scene);
        Transform* GetSelectedEntity() const { return SelectedEntity; }
        void AddEntityToScene(Scene* scene, Entity* parent);
        void AddModelToScene(Scene* scene, Entity* parent);
        void AddAnimatedModelToScene(Scene* scene, Entity* parent);
    };
} // namespace Engine
#endif
