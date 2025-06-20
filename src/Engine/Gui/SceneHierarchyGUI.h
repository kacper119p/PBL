#pragma once
#if EDITOR
#include "Engine/Components/Transform.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/EngineObjects/GizmoManager.h"
#include <algorithm>

namespace Engine
{
    class SceneHierarchyGUI
    {
    private:
        Transform* Root;
        Transform* SelectedEntity;
        static SceneHierarchyGUI* Instance;
        std::string SelectedPrefabPath;

    public:
        SceneHierarchyGUI() = default;

        SceneHierarchyGUI(Transform* Root);

        static SceneHierarchyGUI* GetInstance()
        {
            if (!Instance)
                Instance = new SceneHierarchyGUI();
            return Instance;
        }

        void SetRoot(Transform* root)
        {
            Root = root;
            SelectedEntity = nullptr;
        }

        void DrawHierarchy(Transform* entity, Scene* scene);

        void Draw(Scene* scene);

        Transform* GetSelectedEntity() const { return SelectedEntity; }

        void SetSelectedEntity(Transform* entity) { SelectedEntity = entity; }

        void AddEntityToScene(Scene* scene, Entity* parent);

        void AddModelToScene(Scene* scene, Entity* parent);

        void AddAnimatedModelToScene(Scene* scene, Entity* parent);

        void AddPrefabToScene(Scene* scene, Entity* parent, std::string prefabPath);

        void SetSelectedPrefabPath(const std::string& path) { SelectedPrefabPath = path; }

        bool IsManagedTransform(Transform* t)
        {
            const auto& managed = GizmoManager::GetInstance()->GetManaged();
            return std::find(managed.begin(), managed.end(), t) != managed.end();
        }
    };
} // namespace Engine
#endif
