#include "TextureManager.h"

#include <algorithm>

#include "Utility/TextureUtilities.h"

namespace Engine
{
    std::unordered_map<std::string, uint32_t> TextureManager::Textures;

    TextureManager::~TextureManager() = default;

    uint32_t TextureManager::GetTexture(const char* Path)
    {
        const std::string path = Path;

        if (const auto iterator = Textures.find(path); iterator != Textures.end())
        {
            return iterator->second;
        }

        const uint32_t textureId = Utility::LoadTexture2DFromFile(Path, GL_RGBA, 4,GL_RGBA);

        Textures.emplace(path, textureId);
        return textureId;
    }

    bool TextureManager::DeleteTexture(const char* Path)
    {
        const std::string path = Path;
        if (const auto iterator = Textures.find(path);
            iterator != Textures.end())
        {
            const uint32_t id = iterator->second;
            glDeleteTextures(1, &id);
            Textures.erase(iterator);
            return true;
        }
        return false;
    }

    bool TextureManager::DeleteTexture(const uint32_t Id)
    {
        for (const auto& pair : Textures)
        {
            if (pair.second == Id)
            {
                const uint32_t id = pair.second;
                glDeleteTextures(1, &id);
                Textures.erase(pair.first);
                return true;
            }
        }
        return false;
    }

    void TextureManager::DeleteAllTextures()
    {
        for (const auto& pair : Textures)
        {
            const uint32_t id = pair.second;
            glDeleteTextures(1, &id);
        }
        Textures.clear();
    }

    bool TextureManager::IsValid(const char* Path)
    {
        return Textures.contains(Path);
    }

    bool TextureManager::IsValid(const uint32_t Id)
    {
        for (const auto& pair : Textures)
        {
            if (pair.second == Id)
            {
                return true;
            }
        }
        return false;
    }

    std::string TextureManager::GetTexturePath(const uint32_t Id)
    {
        for (const auto& pair : Textures)
        {
            if (pair.second == Id)
            {
                return pair.first;
            }
        }

        return std::string();
    }
} // Engine
