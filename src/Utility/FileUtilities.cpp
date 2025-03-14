#include <sstream>
#include <fstream>
#include "FileUtilities.h"

namespace Utility
{
    bool GetTextFileContent(const char* path, std::string& destination)
    {
        std::string content;
        std::ifstream fileStream;

        fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            fileStream.open(path);
            std::stringstream stringStream;
            stringStream << fileStream.rdbuf();
            fileStream.close();
            content = stringStream.str();
        } catch (std::ifstream::failure& e)
        {
            fileStream.close();
            return false;
        }
        destination = content;
        return true;
    }
} // Utility
