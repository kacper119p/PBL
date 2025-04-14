#pragma once

#include <string>
#include <vector>

namespace Engine
{
    class AssetsWindow
    {
    public:
        void Draw();

    private:
        void ListDirectory(const std::string& path);
        std::string m_CurrentPath = "./res"; // Start at project assets
        std::vector<std::string> m_DirectoryEntries;

    };
} // namespace Engine
