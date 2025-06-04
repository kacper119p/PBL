#include "PrefabWindow.h"
#include "imgui.h"
#include <filesystem>
#include "Engine/Gui/SceneHierarchyGUI.h"
namespace Engine
{
    void PrefabWindow::DrawImGui(Scene* scene)
    {
        ImGui::Begin("Material Creator");
        ImGui::Separator();
        static char prefabNameBuffer[256] = "Prefab";
        ImGui::InputText("Prefab Name", prefabNameBuffer, sizeof(prefabNameBuffer));
        prefabName = prefabNameBuffer;

        // --- Entity selection ---
        auto collectEntities = [](Entity* entity, std::vector<Entity*>& outEntities, auto&& self) -> void
        {
            if (!entity)
                return;
            outEntities.push_back(entity);
            Transform* transform = entity->GetTransform();
            if (transform)
            {
                const std::vector<Transform*>& children = transform->GetChildren();
                for (Transform* child : children)
                {
                    Entity* childEntity = child->GetOwner();
                    if (childEntity)
                    {
                        self(childEntity, outEntities, self);
                    }
                }
            }
        };

        std::vector<Entity*> entities;
        if (scene)
        {
            Entity* root = scene->GetRoot();
            collectEntities(root, entities, collectEntities);
        }
        // --- Entity selection end ---
        static Entity* selectedEntity = nullptr;
        ImGui::Text("Select Entity to Save as Prefab:");
        if (ImGui::BeginCombo("##EntitySelector", selectedEntity ? selectedEntity->GetName().c_str() : "None"))
        {
            for (Entity* entity : entities)
            {
                bool isSelected = (entity == selectedEntity);
                if (ImGui::Selectable(entity->GetName().c_str(), isSelected))
                {
                    selectedEntity = entity;
                }
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        if (ImGui::Button("Save Prefab"))
        {
            if (selectedEntity)
            {
                std::string prefabFullPath = prefabPath + prefabName + ".prefab";
                PrefabLoader::SavePrefabToFile(prefabFullPath, selectedEntity);
                ImGui::Text("Prefab saved to: %s", prefabFullPath.c_str());
            }
            else
            {
                ImGui::Text("No entity selected to save as prefab.");
            }
        }

        ImGui::Separator();
        ImGui::Text("Select prefab to create:");

        static bool scanned = false;
        std::string prefabsPath = std::filesystem::absolute("./res/prefabs").string();
        static std::vector<std::string> availablePrefabs;
        if (!scanned)
        {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(prefabsPath))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".prefab")
                {
                    std::string relPath = entry.path().lexically_normal().string();
                    std::replace(relPath.begin(), relPath.end(), '\\', '/');
                    size_t pos = relPath.find("/res/");
                    if (pos != std::string::npos)
                        relPath = "." + relPath.substr(pos);
                    availablePrefabs.emplace_back(relPath);
                }
            }
            scanned = true;
        }
        if (ImGui::BeginCombo("##PrefabSelector", "Select Prefab"))
        {
            for (const auto& prefab : availablePrefabs)
            {
                if (ImGui::Selectable(prefab.c_str()))
                {
                    currentPrefabPath = prefab;
                    SceneHierarchyGUI::GetInstance()->SetSelectedPrefabPath(currentPrefabPath);
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::Text("Current Prefab Path: %s", currentPrefabPath.c_str());

        ImGui::End();
    }
} // namespace Engine
