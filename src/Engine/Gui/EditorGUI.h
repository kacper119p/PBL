#pragma once

#if EDITOR
#include <memory>
#include "SceneHierarchyGUI.h"
#include "SceneViewGUI.h"
#include "AssetsWindowGUI.h"
#include "Topbar.h"
#include "Audio/AudioManager.h"
#include "Generation/GenerativeSystem.h"
#include "Models/ModelManager.h"
#include "Materials/MaterialManager.h"

namespace Engine
{
    class EditorGUI
    {
    public:
        EditorGUI() = default;

        ~EditorGUI() = default;

        void Init(); // Call once on startup
        void Render(uint64_t Frame, Scene* scene); // Call every frame
        void SetHierarchyRoot(Transform* root) { m_Hierarchy.SetRoot(root); }

        void SetSceneViewFramebuffer(GLuint Texture)
        {
            m_Scene.SetFramebufferTexture(Texture);
        }

        void RenderInspector(uint64_t Frame, Scene* Scene);

        void DrawSelectedEntitysComponents();

        void DrawGenerativeSystem(Scene* Scene);

        void DrawModelDropZoneAndList(std::vector<std::pair<Models::Model*, Materials::Material*>>& Models,
                                      Models::ModelManager* ModelManager, Materials::MaterialManager* MaterialManager,
                                      const char* UniqueId);

        void DrawModelDropZoneAndList(std::vector<Models::Model*>& Models, std::vector<Materials::Material*>& Materials,
                                      Models::ModelManager* ModelManager, Materials::MaterialManager* MaterialManager,
                                      const char* UniqueId);

    private:
        void SetupDockspace();

    private:
        bool m_Initialized = false;

        SceneHierarchyGUI m_Hierarchy;
        SceneViewGUI m_Scene;
        AudioManager* m_AudioManager;
        AssetsWindow m_AssetsWindow;
        TopBar m_TopBar;
        Generation::GenerativeSystem GenerativeSystem;
        std::unique_ptr<Models::ModelManager> ModelManager;
        std::unique_ptr<Materials::MaterialManager> MaterialManager;

    };

} // namespace Engine
#endif
