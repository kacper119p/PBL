#pragma once

#include <string>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;
namespace Engine
{
    class AssetsWindow
    {
    public:
        void Draw();

    private:
        void ListDirectory(const std::string& path);
        std::string m_CurrentPath = fs::absolute("./res").string(); // Start at project assets
        std::vector<std::string> m_DirectoryEntries;
        std::string m_SearchQuery;
    };
} // namespace Engine
