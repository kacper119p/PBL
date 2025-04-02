#pragma once

#include <cstdint>
#include "glad/glad.h"

namespace Utility
{
    [[nodiscard]] unsigned int LoadTexture2DFromFile(const char* FilePath, GLenum Format,
                                                     uint8_t SourceChannels, GLenum SourceFormat);

    [[nodiscard]] unsigned int LoadHdrCubeMapFromFile(const char* FilePath);

    [[nodiscard]] unsigned int IrradianceMapFromEnvironmentMap(unsigned int EnvironmentMap);

    [[nodiscard]] unsigned int IblPrefilterMapFromEnvironmentMap(unsigned int EnvironmentMap);

    [[nodiscard]] unsigned int GenerateIblBrdfLut();
} // Utility
