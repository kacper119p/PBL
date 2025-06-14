#include "Serialization/SerializationUtility.h"
#if EDITOR
#include "EditorGUI.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Utility/SystemUtilities.h"
#include "Engine/Engine.h"
#include "Engine/EngineObjects/Scene/SceneManager.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/Components/Updateable.h"
#include "Engine/Components/Renderers/ParticleEmitter.h"
#include "Engine/Components/Audio/AudioSource.h"
#include "Engine/EngineObjects/Scene/SceneManager.h"
#include "Engine/EngineObjects/GameMode/GameMode.h"
#include "Engine/EngineObjects/Player/Player.h"
#include "Engine/UI/UiSerializationFactory.h"
#include <filesystem>
#include <random>
#include "spdlog/spdlog.h"
#include "Engine/Components/Camera/CameraFollow.h"
namespace fs = std::filesystem;

void Engine::EditorGUI::Init()
{
}

void Engine::EditorGUI::Render(uint64_t Frame, Scene* scene, CameraRenderData renderData)
{
    SetupDockspace();
    SceneHierarchyGUI::GetInstance()->Draw(scene);
    m_AssetsWindow.Draw();
    m_Scene.Draw(renderData, scene);
    RenderInspector(Frame, scene);
    AudioManager::GetInstance().RenderGlobalVolumeImGui();
    DrawSelectedEntitysComponents();
    DrawGenerativeSystem(scene);
    m_MaterialMenu.DrawMaterialEditor();
    m_PrefabWindow.DrawImGui(scene);

    //m_TopBar.Draw();

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

        static char editableSceneName[256] = {};
        static std::string lastScenePath;

        std::string currentScenePath = scene->GetPath();
        if (currentScenePath != lastScenePath)
        {
            strncpy_s(editableSceneName, fs::path(currentScenePath).filename().string().c_str(),
                      sizeof(editableSceneName) - 1);
            lastScenePath = currentScenePath;
        }

        ImGui::InputText("Scene File Name", editableSceneName, sizeof(editableSceneName));

        if (ImGui::Button("Save Scene"))
        {
            if (!std::string(editableSceneName).empty())
            {
                fs::path newScenePath = fs::path("./res/scenes");
                SceneManager::SaveScene(newScenePath.string() + "/" + editableSceneName, scene);
            }
        }

        ImGui::Separator();
        // gamemode setter
        static std::string selectedGamemodeType = "Select Gamemode Type";
        static std::vector<std::string> gamemodeTypes = GameModeFactory::GetAvailableComponents();

        if (ImGui::BeginCombo("Gamemode Type", selectedGamemodeType.c_str()))
        {
            for (const auto& gamemodeType : gamemodeTypes)
            {
                bool isSelected = (selectedGamemodeType == gamemodeType);
                if (ImGui::Selectable(gamemodeType.c_str(), isSelected))
                {
                    selectedGamemodeType = gamemodeType;
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Add Gamemode"))
        {
            if (!selectedGamemodeType.empty() && selectedGamemodeType != "Select Gamemode Type")
            {
                GameMode* newGameMode = GameModeFactory::CreateObject(selectedGamemodeType);
                scene->SetGameMode(newGameMode);
                selectedGamemodeType = "Select Gamemode Type";
            }
        }

        ImGui::Separator();

        // player setter
        static std::string selectedPlayerType = "Select Player Type";
        static std::vector<std::string> playerTypes = PlayerFactory::GetAvailableComponents();
        if (ImGui::BeginCombo("Player Type", selectedPlayerType.c_str()))
        {
            for (const auto& playerType : playerTypes)
            {
                bool isSelected = (selectedPlayerType == playerType);
                if (ImGui::Selectable(playerType.c_str(), isSelected))
                {
                    selectedPlayerType = playerType;
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Add Player"))
        {
            if (!selectedPlayerType.empty() && selectedPlayerType != "Select Player Type")
            {
                Player* newPlayer = PlayerFactory::CreateObject(selectedPlayerType);
                scene->SetPlayer(newPlayer);
                selectedPlayerType = "Select Player Type";
            }
        }

        ImGui::Separator();

        // UI setter
        static std::string selectedUiType = "Select UI Type";
        static std::vector<std::string> uiTypes = Ui::UiSerializationFactory::GetAvailableUis();

        if (ImGui::BeginCombo("UI Type", selectedUiType.c_str()))
        {
            for (const auto& uiType : uiTypes)
            {
                bool isSelected = (selectedUiType == uiType);
                if (ImGui::Selectable(uiType.c_str(), isSelected))
                {
                    selectedUiType = uiType;
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Add UI"))
        {
            if (!selectedUiType.empty() && selectedUiType != "Select UI Type")
            {
                Ui::Ui* newUi = Ui::UiSerializationFactory::CreateObject(selectedUiType);
                scene->SetUi(newUi);
                selectedUiType = "Select UI Type";
            }
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

    ImGui::Separator();

    CameraFollow::GetInstance().DrawImGui();

    ImGui::End();
}

void Engine::EditorGUI::DrawSelectedEntitysComponents()
{
    static int selectedComponent = -1;

    ImGui::Begin("Components");

    auto selectedEntity = SceneHierarchyGUI::GetInstance()->GetSelectedEntity();
    if (selectedEntity)
    {
        Entity* owner = selectedEntity->GetOwner();
        std::string currentName = owner->GetName();
        char buffer[256] = {};
        strncpy_s(buffer, currentName.c_str(), sizeof(buffer) - 1);

        if (ImGui::InputText("Name", buffer, sizeof(buffer)))
        {
            owner->SetName(std::string(buffer));
        }

        owner->DrawImGui();
    }

    ImGui::Separator();
    ImGui::Text("Add New Component");
    if (ImGui::BeginCombo("##component_combo", selectedComponent >= 0
                                                   ? Serialization::SerializedObjectFactory::GetAvailableComponents()->
                                                   at(selectedComponent).c_str()
                                                   : "Select component..."))
    {
        for (int i = 0; i < Serialization::SerializedObjectFactory::GetAvailableComponents()->size(); ++i)
        {
            bool isSelected = (selectedComponent == i);
            if (ImGui::Selectable(Serialization::SerializedObjectFactory::GetAvailableComponents()->at(i).c_str(),
                                  isSelected))
                selectedComponent = i;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    if (ImGui::Button("Add Component") && selectedComponent >= 0)
    {
        const std::string& typeName = Serialization::SerializedObjectFactory::GetAvailableComponents()->at(
                selectedComponent);
        Component* component = static_cast<Component*>(Serialization::SerializedObjectFactory::CreateObject(typeName));
        selectedEntity->GetOwner()->AddComponent(component);

        selectedComponent = -1; // Reset selection
    }
    ImGui::End();
}

void Engine::EditorGUI::DrawGenerativeSystem(Scene* Scene)
{
    ImGui::Begin("Generative System");

    static int itemsCount = 10;
    static float itemsSpacing = 1.0f;
    static std::vector<float> itemsPercentages;
    static float itemsSizeMin = 1.0;
    static float itemsSizeMax = 1.0;

    static std::vector<std::string> items;

    float totalPercentage = 0.0f;

    DrawPrefabList(items, "Items");

    ImGui::SeparatorText("Items Settings");
    ImGui::SliderInt("Item Count", &itemsCount, 0, 100);
    ImGui::SliderFloat("Item Spacing", &itemsSpacing, 0.1f, 20.0f);
    if (!items.empty())
    {
        if (itemsPercentages.size() != items.size())
            itemsPercentages = std::vector<float>(items.size(), 100.0f / items.size());

        for (int i = 0; i < items.size(); ++i)
        {
            std::string label = "Item " + std::to_string(i) + " Percentage";

            ImGui::InputFloat(label.c_str(), &itemsPercentages[i], 0.0f, 100.0f);
            totalPercentage += itemsPercentages[i];
        }

        if (std::round(totalPercentage) != 100.0f)
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));

        ImGui::Text("Total: %.1f%%", totalPercentage);

        if (std::round(totalPercentage) != 100.0f)
            ImGui::PopStyleColor();
    }
    ImGui::SliderFloat("Item Size Min", &itemsSizeMin, 0.1f, 2.0f);
    if (itemsSizeMax < itemsSizeMin)
        itemsSizeMax = itemsSizeMin;
    ImGui::SliderFloat("Item Size Max", &itemsSizeMax, 0.1f, 2.0f);

    if (ImGui::Button("Generate Items"))
    {
        if (!items.empty() && std::round(totalPercentage) == 100.0f)
        {
            GenerativeSystem.GenerateItems(Scene, items, itemsCount, itemsSpacing, itemsPercentages, itemsSizeMin,
                                           itemsSizeMax);
        }
    }

    ImGui::End();
}

void Engine::EditorGUI::DrawPrefabList(std::vector<std::string>& Prefabs, const char* UniqueId)
{
    static std::string draggedPrefab;

    ImGui::SeparatorText("Generated Items");

    const std::string childId = "PrefabList##" + std::string(UniqueId);
    ImGui::BeginChild(childId.c_str(), ImVec2(0, 150), true);

    if (Prefabs.empty())
        ImGui::Text("Drop Prefab Here");

    for (int i = 0; i < Prefabs.size(); ++i)
    {
        ImGui::PushID(i);

        std::filesystem::path fsPath(Prefabs[i]);
        std::string filename = fsPath.filename().stem().string();

        ImGui::Text("%d. %s", i, filename.c_str());

        ImGui::SameLine();
        if (ImGui::Button("Remove"))
        {
            Prefabs.erase(Prefabs.begin() + i);
            ImGui::PopID();
            break;
        }

        ImGui::PopID();
    }

    ImGui::EndChild();

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
        {
            std::string fullPath(static_cast<const char*>(payload->Data));
            std::filesystem::path fsPath(fullPath);
            std::string filename = fsPath.filename().string();

            if (filename.ends_with(".prefab"))
            {
                draggedPrefab = fullPath;
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (!draggedPrefab.empty())
    {
        Prefabs.emplace_back(std::move(draggedPrefab));
        draggedPrefab.clear();
    }
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
        ImGui::DockBuilderDockWindow("Generative System", dock_right_top);
        ImGui::DockBuilderDockWindow("Material Creator", dock_right_top);
        ImGui::DockBuilderDockWindow("Assets", dock_bottom);
        ImGui::DockBuilderDockWindow("Scene", dock_main);


        ImGui::DockBuilderFinish(dockspace_id);
        m_Initialized = true;
    }

    ImGui::End();
}
#endif
