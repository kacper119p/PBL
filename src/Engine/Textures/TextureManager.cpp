#include "TextureManager.h"

#include <algorithm>

#include "Texture.h"
#include "Utility/TextureUtilities.h"


namespace Engine
{
    std::unordered_map<std::string, Texture> TextureManager::Textures;

    Texture TextureManager::GetTexture(const char* Path)
    {
        const std::string path = Path;

        if (const auto iterator = Textures.find(path); iterator != Textures.end())
        {
            return Texture(iterator->second);
        }

        const uint32_t textureId = Utility::LoadTexture2DFromFile(Path, GL_RGBA, 4,GL_RGBA);

        Textures.emplace(path, textureId);
        return Texture(textureId);
    }

    bool TextureManager::DeleteTexture(const char* Path)
    {
        const std::string path = Path;
        if (const auto iterator = Textures.find(path);
            iterator != Textures.end())
        {
            const uint32_t id = iterator->second.GetId();
            glDeleteTextures(1, &id);
            Textures.erase(iterator);
            return true;
        }
        return false;
    }

    bool TextureManager::DeleteTexture(const Texture Texture)
    {
        for (const auto& pair : Textures)
        {
            if (pair.second == Texture)
            {
                const uint32_t id = pair.second.GetId();
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
            const uint32_t id = pair.second.GetId();
            glDeleteTextures(1, &id);
        }
        Textures.clear();
    }

    bool TextureManager::IsValid(const char* Path)
    {
        return Textures.contains(Path);
    }

    bool TextureManager::IsValid(const Texture Texture)
    {
        for (const auto& pair : Textures)
        {
            if (pair.second == Texture)
            {
                return true;
            }
        }
        return false;
    }

    std::string TextureManager::GetTexturePath(const Texture Texture)
    {
        for (const auto& pair : Textures)
        {
            if (pair.second == Texture)
            {
                return pair.first;
            }
        }

        return std::string();
    }
} // Engine
