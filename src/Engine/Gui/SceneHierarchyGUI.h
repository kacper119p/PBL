#pragma once
#if EDITOR
#include <unordered_set>
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
        std::unordered_set<Transform*> SelectedEntities;
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

        Transform* GetRoot() { return Root; }

        void SetRoot(Transform* root)
        {
            Root = root;
            SelectedEntities.clear();
        }

        void DrawHierarchy(Transform* entity, Scene* scene);

        void Draw(Scene* scene);

        std::unordered_set<Transform*>& GetSelectedEntities() { return SelectedEntities; }

        void SetSelectedEntities(const std::unordered_set<Transform*>& entities)
        {
            SelectedEntities = entities;
        }

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
