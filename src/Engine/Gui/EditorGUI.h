#pragma once

#if EDITOR
#include <memory>
#include "SceneHierarchyGUI.h"
#include "SceneViewGUI.h"
#include "AssetsWindowGUI.h"
#include "Topbar.h"
#include "MaterialsMenu.h"
#include "Audio/AudioManager.h"
#include "Generation/GenerativeSystem.h"
#include "Models/ModelManager.h"
#include "Materials/MaterialManager.h"
#include "PrefabWindow.h"

namespace Engine
{
    class EditorGUI
    {
    public:
        EditorGUI() = default;

        ~EditorGUI() = default;

        void Init(); // Call once on startup
        void Render(uint64_t Frame, Scene* scene, CameraRenderData renderData); // Call every frame
        void SetHierarchyRoot(Transform* root) { SceneHierarchyGUI::GetInstance()->SetRoot(root); }

        void SetSceneViewFramebuffer(GLuint Texture)
        {
            m_Scene.SetFramebufferTexture(Texture);
        }

        void RenderInspector(uint64_t Frame, Scene* Scene);

        void DrawSelectedEntitysComponents();

        void DrawGenerativeSystem(Scene* Scene);

        void DrawPrefabList(std::vector<std::string>& Prefabs, const char* UniqueId);

    private:
        void SetupDockspace();

    private:
        bool m_Initialized = false;

        SceneViewGUI m_Scene;
        AudioManager* m_AudioManager;
        AssetsWindow m_AssetsWindow;
        MaterialsMenu m_MaterialMenu;
        TopBar m_TopBar;
        PrefabWindow m_PrefabWindow;
        Generation::GenerativeSystem GenerativeSystem;

    };

} // namespace Engine
#endif
