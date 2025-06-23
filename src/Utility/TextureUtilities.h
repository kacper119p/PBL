#pragma once

#include <cstdint>
#include "glad/glad.h"

namespace Utility
{
    /**
     * @brief Loads a DDS texture from file.
     * @param FilePath Path to the DDS texture file.
     * @return OpenGL texture ID.
     */
    [[nodiscard]] unsigned int LoadTexture2DFromFile(const char* FilePath);

    /**
     * @brief Loads a DDS texture and returns its dimensions.
     * @param FilePath Path to the DDS texture file.
     * @param OutWidth Output texture width.
     * @param OutHeight Output texture height.
     * @return OpenGL texture ID.
     */
    [[nodiscard]] unsigned int LoadTexture2DFromFile(const char* FilePath, int& OutWidth, int& OutHeight);

    /**
    * @brief Loads an HDR equirectangular image and converts it to a cubemap.
    * @param FilePath Path to the HDR image.
    * @return Cubemap texture ID.
    */
    [[nodiscard]] unsigned int LoadHdrCubeMapFromFile(const char* FilePath);

    /**
     * @brief Generates an irradiance cubemap from an environment map.
     * @param EnvironmentMap Input environment cubemap texture ID.
     * @return Irradiance cubemap texture ID.
     */
    [[nodiscard]] unsigned int IrradianceMapFromEnvironmentMap(unsigned int EnvironmentMap);

    /**
     * @brief Generates a prefiltered IBL cubemap from an environment map.
     * @param EnvironmentMap Input environment cubemap texture ID.
     * @return Prefiltered cubemap texture ID.
     */
    [[nodiscard]] unsigned int IblPrefilterMapFromEnvironmentMap(unsigned int EnvironmentMap);

    /**
    * @brief Generates a BRDF lookup texture for IBL specular reflection.
    * @return 2D LUT texture ID.
    */
    [[nodiscard]] unsigned int GenerateIblBrdfLut();
} // Utility
