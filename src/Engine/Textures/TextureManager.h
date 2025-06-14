#pragma once
#include <string>
#include <unordered_map>

namespace Engine
{
    class Texture;
}

namespace Engine
{
    /**
     * @brief Class containing static methods for loading and managing textures.
     */
    class TextureManager final
    {
    private:
        static std::unordered_map<std::string, Texture> Textures;

    private:
        TextureManager() = default;

    public:
        /**
         * @brief Loads a texture from a file if not already loaded, otherwise retrieves an id of an already loaded texture.
         * @param Path Path to a texture file.
         * @return A loaded texture.
         */
        static Texture GetTexture(const char* Path);

        /**
         * @brief Frees resources used by an existing texture.
         * @param Path Path to a texture file.
         * @return True if the texture existed prior to a method call. False otherwise.
         */
        static bool DeleteTexture(const char* Path);

        /**
         * @brief Frees resources used by an existing texture.
         * @param Texture Texture to delete
         * @return True if a texture existed prior to a method call. False otherwise.
         */
        static bool DeleteTexture(Texture Texture);

        /**
         * @brief Frees resources used by all textures.
         */
        static void DeleteAllTextures();

        /**
         * @brief Checks if a texture is loaded and ready to use.
         * @param Path Path to a texture file.
         * @return True if a texture is loaded. False otherwise.
         */
        static bool IsValid(const char* Path);

        /**
         * @brief Checks if a texture is loaded and ready to use.
         * @param Texture Texture to be checked.
         * @return True if a texture is loaded. False otherwise.
         */
        static bool IsValid(Texture Texture);

        /**
         * @brief Checks if a texture is loaded and ready to use.
         * @param Texture Texture to be found.
         * @return
         */
        static std::string GetTexturePath(Texture Texture);
    };
} // Engine
