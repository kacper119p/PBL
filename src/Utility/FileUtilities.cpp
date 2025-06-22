#include <sstream>
#include <fstream>
#include "FileUtilities.h"
#include <windows.h>
#include <iostream>
#include <algorithm>

namespace Utility
{
    bool GetTextFileContent(const char* Path, std::string& Destination)
    {
        std::string content;
        std::ifstream fileStream;

        fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            fileStream.open(Path);
            std::stringstream stringStream;
            stringStream << fileStream.rdbuf();
            fileStream.close();
            content = stringStream.str();
        } catch (std::ifstream::failure& e)
        {
            return false;
        }
        Destination = content;
        return true;
    }

    void FindFilesWithExtension(const std::string& Directory, const std::string& Extension,
                                std::vector<std::string>& Results)
    {
        const std::string searchPath = Directory + "\\*";

        WIN32_FIND_DATAA findData;
        const HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

        if (hFind == INVALID_HANDLE_VALUE)
            return;

        do
        {
            std::string name = findData.cFileName;

            if (name == "." || name == "..")
                continue;

            std::string fullPath = Directory + "\\" + name;

            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                FindFilesWithExtension(fullPath, Extension, Results);
            }
            else
            {
                const size_t dotPos = name.rfind('.');
                if (dotPos != std::string::npos)
                {
                    std::string fileExt = name.substr(dotPos);

                    std::ranges::transform(fileExt, fileExt.begin(), ::tolower);
                    std::string lowerExt = Extension;
                    std::ranges::transform(lowerExt, lowerExt.begin(), ::tolower);

                    if (fileExt == lowerExt)
                    {
                        Results.push_back(fullPath);
                    }
                }
            }

        } while (FindNextFileA(hFind, &findData) != 0);

        FindClose(hFind);
    }
} // Utility
