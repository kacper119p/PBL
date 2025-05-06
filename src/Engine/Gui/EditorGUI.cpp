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
#include <filesystem>
#include <iostream>
#include <random>
#include "spdlog/spdlog.h"
namespace fs = std::filesystem;

void Engine::EditorGUI::Init()
{
}

void Engine::EditorGUI::Render(uint64_t Frame, Scene* scene)
{
    SetupDockspace();
    m_Hierarchy.Draw(scene);
    m_AssetsWindow.Draw();
    m_Scene.Draw();
    RenderInspector(Frame, scene);
    AudioManager::GetInstance().RenderGlobalVolumeImGui();
    DrawSelectedEntitysComponents();
    DrawGenerativeSystem(scene);
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
    static int selectedComponent = -1;

    ImGui::Begin("Components");

    auto selectedEntity = m_Hierarchy.GetSelectedEntity();
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

void Engine::EditorGUI::DrawGenerativeSystem(Scene* scene)
{
    ImGui::Begin("Generative System");

    static int trashCount = 10;
    static float trashSpacing = 1.0f;
    static std::vector<float> trashPercentages;

    static int bloodCount = 5;
    static float bloodSpacing = 2.0f;
    static float bloodSize = 1.0f;

    static std::vector<std::pair<Models::Model*, Materials::Material*>> trash;
    static std::vector<std::pair<Models::Model*, Materials::Material*>> stains;

    float totalPercentage = 0.0f;

    DrawModelDropZoneAndList(trash, modelManager.get(), materialManager.get(), "Trash");

    ImGui::SeparatorText("Trash Settings");
    ImGui::SliderInt("Trash Count", &trashCount, 0, 100);
    ImGui::SliderFloat("Trash Spacingy", &trashSpacing, 0.1f, 20.0f);
    if (trash.size() > 0)
    {
        if (trashPercentages.size() != trash.size())
            trashPercentages = std::vector<float>(trash.size(), 100.0f / trash.size());

        for (int i = 0; i < trash.size(); ++i)
        {
            std::string fullPath = trash.at(i).first->GetPath();
            std::filesystem::path fsPath(fullPath);
            std::string filename = fsPath.filename().string() + " Percentage";
            ImGui::InputFloat(filename.c_str(), &trashPercentages[i], 0.0f, 100.0f);
            totalPercentage += trashPercentages[i];
        }

        if (std::ceil(totalPercentage) != 100.0f)
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));

        ImGui::Text("Total: %.1f%%", totalPercentage);

        if (std::ceil(totalPercentage) != 100.0f)
            ImGui::PopStyleColor();
    }

    DrawModelDropZoneAndList(stains, modelManager.get(), materialManager.get(), "Stains");

    ImGui::SeparatorText("Blood Settings");
    ImGui::SliderInt("Blood Count", &bloodCount, 0, 50);
    ImGui::SliderFloat("Blood Spacing", &bloodSpacing, 0.1f, 20.0f);
    ImGui::SliderFloat("Blood Size", &bloodSize, 0.1f, 5.0f);

    if (ImGui::Button("Generate Trash"))
    {
        LastGeneratedEntity = nullptr;

        if (!trash.empty() && std::ceil(totalPercentage) == 100.0f)
        {
            m_GenerativeSystem.GenerateTrash(scene, trash, trashCount, trashSpacing, trashPercentages,
                                             LastGeneratedEntity);
        }
    }
    if (ImGui::Button("Generate Stains"))
    {
        LastGeneratedEntity = nullptr;

        if (!stains.empty())
        {
            m_GenerativeSystem.GenerateBlood(scene, stains, bloodCount, bloodSize, bloodSpacing, LastGeneratedEntity);
        }
    }

    if (ImGui::Button("Clear Last Generated"))
    {
        m_GenerativeSystem.ClearGeneratedEntities(LastGeneratedEntity);
    }

    ImGui::End();
}

void Engine::EditorGUI::DrawModelDropZoneAndList(
        std::vector<std::pair<Models::Model*, Materials::Material*>>& modelsAndMaterials,
        Models::ModelManager* modelManager,
        Materials::MaterialManager* materialManager,
        const char* uniqueId)
{
    static Models::Model* draggedModel = nullptr;
    static Materials::Material* draggedMaterial = nullptr;

    ImGui::SeparatorText("Drag & Drop Model and Material Here");

    std::string modelButtonLabel = "Drop Model Here##" + std::string(uniqueId);
    ImGui::Button(modelButtonLabel.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 50));

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
        {
            std::string fullPath(static_cast<const char*>(payload->Data));
            std::filesystem::path fsPath(fullPath);
            std::string filename = fsPath.filename().string();

            if (filename.ends_with(".fbx") || filename.ends_with(".obj") || filename.ends_with(".gltf"))
            {
                draggedModel = modelManager->GetModel(fullPath.c_str());
                if (draggedModel)
                {
                    spdlog::info("Model added: {}", filename);
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    std::string materialButtonLabel = "Drop Material Here##" + std::string(uniqueId);
    ImGui::Button(materialButtonLabel.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 50));

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
        {
            std::string fullPath(static_cast<const char*>(payload->Data));
            std::filesystem::path fsPath(fullPath);
            std::string filename = fsPath.filename().string();

            if (filename.ends_with(".mat"))
            {
                draggedMaterial = materialManager->GetMaterial(fullPath.c_str());
                if (draggedMaterial)
                {
                    spdlog::info("Material added: {}", filename);
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (draggedModel && draggedMaterial)
    {
        modelsAndMaterials.push_back(std::make_pair(draggedModel, draggedMaterial));
        spdlog::info("Model and Material successfully added to the list.");

        draggedModel = nullptr;
        draggedMaterial = nullptr;
    }

    ImGui::SeparatorText("Prefab List");
    for (int i = 0; i < modelsAndMaterials.size(); ++i)
    {
        ImGui::PushID(i);

        std::string modelPath = modelsAndMaterials[i].first->GetPath();
        std::filesystem::path fsPath(modelPath);
        std::string modelFilename = fsPath.filename().string();

        char itemBuffer[256];
        strncpy(itemBuffer, modelFilename.c_str(), sizeof(itemBuffer));
        itemBuffer[sizeof(itemBuffer) - 1] = '\0';

        ImGui::InputText("##ModelPath", itemBuffer, sizeof(itemBuffer), ImGuiInputTextFlags_ReadOnly);

        std::string materialName = modelsAndMaterials[i].second
                                       ? modelsAndMaterials[i].second->GetType()
                                       : "No Material";
        ImGui::Text("Material: %s", materialName.c_str());

        ImGui::SameLine();
        if (ImGui::Button("Remove"))
        {
            modelsAndMaterials.erase(modelsAndMaterials.begin() + i);
            ImGui::PopID();
            break;
        }

        ImGui::PopID();
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
        ImGui::DockBuilderDockWindow("Assets", dock_bottom);
        ImGui::DockBuilderDockWindow("Scene", dock_main);


        ImGui::DockBuilderFinish(dockspace_id);
        m_Initialized = true;
    }

    ImGui::End();
}
#endif
