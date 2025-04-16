#include "EditorGUI.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Utility/SystemUtilities.h"
#include "Engine/Engine.h"
#include "Engine/EngineObjects/Scene/SceneManager.h"
#include "Scene/SceneBuilder.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/Components/Updateable.h"
#include "Engine/Components/Renderers/ParticleEmitter.h"
#include "Engine/Components/Renderers/ModelRenderer.h"
#include "Engine/Components/Audio/AudioSource.h"
#include "Engine/Components/Game/Rotator.h"
#include "Engine/Components/Game/ShipRoller.h"
#include <filesystem>
namespace fs = std::filesystem;

void Engine::EditorGUI::Init() { }
void Engine::EditorGUI::Render(uint64_t Frame, Scene* scene)
{ 
    SetupDockspace();
    m_Hierarchy.Draw(scene);
    m_AssetsWindow.Draw();
    m_Scene.Draw();
    RenderInspector(Frame, scene);
    AudioManager::GetInstance().RenderGlobalVolumeImGui();
    DrawSelectedEntitysComponents();
    m_TopBar.Draw();
    
}
void Engine::EditorGUI::RenderInspector(uint64_t Frame, Scene* scene)
{
    ImGui::Begin("Engine Properties");
    if (ImGui::CollapsingHeader("Inspector", ImGuiTreeNodeFlags_DefaultOpen))
    {
        //for scene selection
        static bool showScenePopup = false;
        static std::vector<std::string> availableScenes;
        static bool scanned = false;
        std::string scenePath = fs::absolute("./res/scenes").string();

        if (!scanned)
        {
            for (const auto& entry : fs::directory_iterator(scenePath))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".lvl")
                    availableScenes.emplace_back(entry.path().string());
            }
            scanned = true;
        }
        ImGui::Text("Current scene:");
        if (scene->GetPath() == "")
        {
            scenePath = "None";
        }
        else
        {
            scenePath = scene->GetPath();
        }
        ImGui::Selectable(scenePath.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);
        if (ImGui::IsItemClicked())
            showScenePopup = true;

        if (showScenePopup)
        {
            ImGui::OpenPopup("Scene Picker");
            showScenePopup = false;
        }

        if (ImGui::BeginPopup("Scene Picker"))
        {
            for (const auto& path : availableScenes)
            {
                std::filesystem::path fsPath(path);
                std::string displayName = fsPath.filename().string();

                if (ImGui::Selectable(displayName.c_str()))
                {
                    LightManager::GetInstance()->ClearAllLights();
                    SceneManager::LoadScene(path, scene);
                    SetHierarchyRoot(scene->GetRoot()->GetTransform());
                }

                ImGui::SameLine();
                ImGui::TextDisabled("(%s)", path.c_str());
            }
            ImGui::EndPopup();
        }

        ImGui::Separator();

        //for inspector
        const GLubyte* renderer = glGetString(GL_RENDERER);
        std::string cpuInfo = Utility::GetCpuInfo();
        int ram = Utility::GetTotalRamGB();
        const GLubyte* version = glGetString(GL_VERSION);
        const GLubyte* shadingLanguageVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

        ImGui::Text("GPU: %s", (char*) renderer);
        ImGui::Text("CPU: %s", cpuInfo.c_str());
        ImGui::Text("RAM: %dGB", ram);
        ImGui::Text("OpenGL version: %s", (char*) version);
        ImGui::Text("Shading Language version: %s", (char*) shadingLanguageVersion);

        ImGui::Text("Frame: %llu\nApplication average %.3f ms/frame\n(%.1f FPS)", Frame,
                    1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }


    ImGui::End();
}
void Engine::EditorGUI::DrawSelectedEntitysComponents()
{
    static std::vector<std::string> availableComponents = { 
                                                           "Particle emmiter",
                                                           "Model renderer",
                                                           "Spot light",
                                                           "Point light",
                                                           "Directional light",
                                                           "Rotator",
                                                           "Ship roller",
                                                           "Audio source"};
    static int selectedComponent = -1;

    ImGui::Begin("Components");

    auto selectedEntity = m_Hierarchy.GetSelectedEntity();
    if (selectedEntity)
    {
        auto owner = selectedEntity->GetOwner();
        std::string currentName = owner->GetName();
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strncpy(buffer, currentName.c_str(), sizeof(buffer) - 1);

        if (ImGui::InputText("Name", buffer, sizeof(buffer)))
        {
            owner->SetName(std::string(buffer));
        }

        owner->DrawImGui();
    }

    ImGui::Separator();
    ImGui::Text("Add New Component");
    if (ImGui::BeginCombo("##component_combo", selectedComponent >= 0 ? availableComponents[selectedComponent].c_str()
                                                                      : "Select component..."))
    {
        for (int i = 0; i < availableComponents.size(); ++i)
        {
            bool isSelected = (selectedComponent == i);
            if (ImGui::Selectable(availableComponents[i].c_str(), isSelected))
                selectedComponent = i;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    if (ImGui::Button("Add Component") && selectedComponent >= 0)
    {
        switch (selectedComponent)
        {
            case 0:
                selectedEntity->GetOwner()->AddComponent<ParticleEmitter>();
                break;
            case 1:
                selectedEntity->GetOwner()->AddComponent<ModelRenderer>();
                break;
            case 2:
                selectedEntity->GetOwner()->AddComponent<SpotLight>();
                break;
            case 3:
                selectedEntity->GetOwner()->AddComponent<PointLight>();
                break;
            case 4:
                selectedEntity->GetOwner()->AddComponent<DirectionalLight>();
                break;
            case 5:
                selectedEntity->GetOwner()->AddComponent<Rotator>();
                break;
            case 6:
                selectedEntity->GetOwner()->AddComponent<ShipRoller>();
                break;
            case 7:
                selectedEntity->GetOwner()->AddComponent<AudioSource>();
                break;
            default:
                break;
        }

        selectedComponent = -1; // Reset selection
    }
    ImGui::End();
}

void Engine::EditorGUI::SetupDockspace()
{
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

     ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0)); // Transparent background

    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();

    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    if (!m_Initialized)
    {
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        ImGuiID dock_main = dockspace_id;
        ImGuiID dock_left = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 0.2f, nullptr, &dock_main);
        ImGuiID dock_right = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.25f, nullptr, &dock_main);
        ImGuiID dock_bottom = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down, 0.25f, nullptr, &dock_main);

        // Create a new dock for the Properties, Audio Manager, and Inspector to be inside one dock
        ImGuiID dock_right_top = ImGui::DockBuilderSplitNode(dock_right, ImGuiDir_Up, 0.6f, nullptr, &dock_right);

        // Dock the windows into the same dock
        ImGui::DockBuilderDockWindow("Hierarchy", dock_left);
        ImGui::DockBuilderDockWindow("Engine Properties", dock_right_top);
        ImGui::DockBuilderDockWindow("Components", dock_right_top);
        ImGui::DockBuilderDockWindow("Assets", dock_bottom);
        ImGui::DockBuilderDockWindow("Scene", dock_main);


        ImGui::DockBuilderFinish(dockspace_id);
        m_Initialized = true;
    }

    ImGui::End();
}
