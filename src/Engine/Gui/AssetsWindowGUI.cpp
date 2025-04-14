#include <algorithm>
#include <filesystem>
#include "imgui.h"
#include "AssetsWindowGUI.h"

namespace fs = std::filesystem;

void Engine::AssetsWindow::Draw()
{
    ImGui::Begin("Assets");

    if (ImGui::Button("Up") && m_CurrentPath != "./assets")
    {
        m_CurrentPath = fs::path(m_CurrentPath).parent_path().string();
    }

    ListDirectory(m_CurrentPath);

    for (const auto& entry : m_DirectoryEntries)
    {
        std::string fullPath = m_CurrentPath + "/" + entry;
        if (fs::is_directory(fullPath))
        {
            if (ImGui::Selectable((entry + "/").c_str(), false))
            {
                m_CurrentPath = fullPath;
            }
        }
        else
        {
            ImGui::Text("%s", entry.c_str());
        }
    }

    ImGui::End();
}

void Engine::AssetsWindow::ListDirectory(const std::string& path)
{
    m_DirectoryEntries.clear();

    try
    {
        for (const auto& entry : fs::directory_iterator(path))
        {
            m_DirectoryEntries.push_back(entry.path().filename().string());
        }

        std::sort(m_DirectoryEntries.begin(), m_DirectoryEntries.end());
    } catch (const std::exception& e)
    {
        m_DirectoryEntries.push_back("[Error reading directory]");
    }
}
