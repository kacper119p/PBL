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
        static std::unordered_map<std::string, uint32_t> Textures;

    public:
        virtual ~TextureManager() = 0;

    public:
        /**
         * @brief Loads a texture from a file if not already loaded, otherwise retrieves an id of an already loaded texture.
         * @param Path Path to a texture file.
         * @return
         */
        static uint32_t GetTexture(const char* Path);

        /**
         * @brief Frees resources used by an existing texture.
         * @param Path Path to a texture file.
         * @return True if the texture existed prior to a method call. False otherwise.
         */
        static bool DeleteTexture(const char* Path);

        /**
         * @brief Frees resources used by an existing texture.
         * @param Id ID of a texture.
         * @return True if a texture existed prior to a method call. False otherwise.
         */
        static bool DeleteTexture(uint32_t Id);

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
         * @param Id ID of a texture.
         * @return True if a texture is loaded. False otherwise.
         */
        static bool IsValid(uint32_t Id);

        /**
         * @brief Checks if a texture is loaded and ready to use.
         * @param Id ID of a texture.
         * @return
         */
        static std::string GetTexturePath(uint32_t Id);
    };
} // Engine
