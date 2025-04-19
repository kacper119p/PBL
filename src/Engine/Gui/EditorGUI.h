#pragma once
#include <memory>
#include "SceneHierarchyGUI.h"
#include "SceneViewGUI.h"
#include "AssetsWindowGUI.h"
#include "Topbar.h"
#include "Audio/AudioManager.h"

namespace Engine {
    class EditorGUI
    {
    public:
        EditorGUI() = default;
        ~EditorGUI()=default;

        void Init(); // Call once on startup
        void Render(uint64_t Frame,Scene* scene); // Call every frame
        void SetHierarchyRoot(Transform* root) { m_Hierarchy.SetRoot(root); }
        void SetSceneViewFramebuffer(GLuint Texture)
        {
            m_Scene.SetFramebufferTexture(Texture);
        }
        void RenderInspector(uint64_t Frame,Scene* scene);
        void DrawSelectedEntitysComponents();
        

    private:
        void SetupDockspace();

    private:
        bool m_Initialized = false;

        SceneHierarchyGUI m_Hierarchy;
        SceneViewGUI m_Scene;
        AudioManager* m_AudioManager;
        AssetsWindow m_AssetsWindow;
        TopBar m_TopBar;

    };

}