#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include "Engine/Textures/TextureManager.h"
#include "Engine/Textures/Texture.h"
#include "Materials/Properties/TextureMaterialProperty.h"
#include "imgui.h"

class Helpers
{
public:
    static inline void GetTextureList(std::vector<std::string>& availableTextures, bool& scanned)
    {
        std::string texturePath = std::filesystem::absolute("./res/textures").string();
        if (!scanned)
        {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(texturePath))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".dds")
                {
                    std::string relativePath = std::filesystem::relative(entry.path(), texturePath).string();
                    std::string pseudoAbsolutePath = "res/textures/" + relativePath;
                    availableTextures.emplace_back(pseudoAbsolutePath);
                }
            }
            scanned = true;
        }
    }
    static inline void ChangeTexture(const char* label, Engine::Texture& texture, bool& showPopup,
                                     const std::vector<std::string>& availableTextures,
                                     const char* popupLabel)
    {
        std::string texturePath = std::filesystem::absolute("./res/textures").string();
        std::string currentPath = Engine::TextureManager::GetTexturePath(texture);

        ImGui::Text("%s", label);
        ImGui::Selectable(currentPath.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
            {
                const char* droppedPath = static_cast<const char*>(payload->Data);
                if (std::filesystem::path(droppedPath).extension() == ".dds")
                {
                    texture = (Engine::TextureManager::GetTexture(droppedPath));
                }
            }
            ImGui::EndDragDropTarget();
        }

        if (ImGui::IsItemClicked())
            showPopup = true;

        if (showPopup)
        {
            ImGui::OpenPopup(popupLabel);
            showPopup = false;
        }

        if (ImGui::BeginPopup(popupLabel))
        {
            for (const auto& path : availableTextures)
            {
                std::filesystem::path fsPath(path);
                std::string displayName = std::filesystem::relative(fsPath, texturePath).string();
                if (ImGui::Selectable(displayName.c_str()))
                {
                    texture = Engine::TextureManager::GetTexture(
                            ("res/textures/" + std::filesystem::relative(path, texturePath).string()).c_str());

                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                ImGui::TextDisabled("(%s)", path.c_str());
            }
            ImGui::EndPopup();
        }
    }
};
