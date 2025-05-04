#include "DDSLoader.h"

#include <cstdio>
#include <cstdlib>
#include <glad/glad.h>
#include <glm/ext/scalar_int_sized.hpp>

#include "../../cmake-build-debug-visual-studio-editor/_deps/spdlog-src/include/spdlog/spdlog.h"
#include "Events/Action.h"

#define EXIT\
    {\
    free(buffer);\
    fclose(f);\
    return textureId;\
    }

#define NOTIFY_FAILED spdlog::error("Loading texture from {0} failed.", FilePath);

namespace Utility
{
    GLuint LoadDds(const char* FilePath, int& Width, int& Height)
    {
        GLuint textureId = 0;

        FILE* f;
        uint8_t* buffer;
        if (fopen_s(&f, FilePath, "rb") != 0)
        {
#if DEBUG
            NOTIFY_FAILED
#endif
            return textureId;
        }

        fseek(f, 0, SEEK_END);
        long file_size = ftell(f);
        fseek(f, 0, SEEK_SET);

        int32_t fileCode;
        fread(&fileCode, 1, 4, f);
#if DEBUG
        if (fileCode != DDS_MAGIC)
        {
            NOTIFY_FAILED
            EXIT
        }
#endif

        DDS_HEADER header;
        DDS_HEADER_DXT10 headerDxt;

        fread(&header, sizeof(DDS_HEADER), 1, f);
        fread(&headerDxt, sizeof(DDS_HEADER_DXT10), 1, f);

        Height = static_cast<int>(header.dwHeight);
        Width = static_cast<int>(header.dwWidth);

        int32_t format;
        int32_t blockSize;

#if DEBUG
        constexpr char DX10FourCC[4] = {'D', 'X', '1', '0'};
        if (memcmp(&header.ddspf.dwFourCC, DX10FourCC, sizeof(DWORD)) != 0)
        {
            NOTIFY_FAILED
            EXIT
        }
#endif
        switch (headerDxt.dxgiFormat)
        {
            case DXGI_FORMAT_BC1_UNORM: // DXT1
                format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                blockSize = 8;
                break;
            case DXGI_FORMAT_BC2_UNORM: // DXT3
                format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                blockSize = 16;
                break;
            case DXGI_FORMAT_BC3_UNORM: // DXT5
                format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                blockSize = 16;
                break;
            case DXGI_FORMAT_BC7_UNORM: // BPTC
                format = GL_COMPRESSED_RGBA_BPTC_UNORM;
                blockSize = 16;
                break;
            case DXGI_FORMAT_R16G16B16A16_UNORM: // uncompressed
                format = GL_RGBA16;
                blockSize = 8;
                break;
            default: // Unsupported format
#if DEBUG
                NOTIFY_FAILED
#endif
                EXIT
        }

        buffer = reinterpret_cast<uint8_t*>(malloc(file_size - sizeof(DDS_HEADER) - sizeof(DDS_HEADER_DXT10)));
        if (buffer == nullptr)
        {
#if DEBUG
            NOTIFY_FAILED
#endif
            EXIT
        }

        fread(buffer, 1, file_size, f);

        glGenTextures(1, &textureId);
        if (textureId == 0)
        {
            NOTIFY_FAILED
            EXIT
        }

        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(header.dwMipMapCount - 1));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16.0f);

        uint32_t offset = 0;
        int32_t size = 0;

        uint32_t mipMapCount = header.dwMipMapCount;

        int32_t h = static_cast<int32_t>(header.dwHeight);
        int32_t w = static_cast<int32_t>(header.dwWidth);

        // loop through sending block at a time with the magic formula
        // upload to opengl properly, note the offset transverses the pointer
        // assumes each mipmap is 1/2 the size of the previous mipmap
        if (headerDxt.dxgiFormat == DXGI_FORMAT_R16G16B16A16_UNORM)
        {
            for (unsigned int i = 0; i < mipMapCount; i++)
            {
                if (w == 0 || h == 0)
                {
                    // discard any odd mipmaps 0x1 0x2 resolutions
                    mipMapCount--;
                    continue;
                }
                size = w * h * blockSize;
                glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(i), format, w, h, 0, GL_RGBA, GL_UNSIGNED_SHORT,
                             buffer + offset);
                offset += size;
                w /= 2;
                h /= 2;
            }
        }
        else
        {
            for (unsigned int i = 0; i < mipMapCount; i++)
            {
                if (w == 0 || h == 0)
                {
                    // discard any odd mipmaps 0x1 0x2 resolutions
                    mipMapCount--;
                    continue;
                }
                size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;
                glCompressedTexImage2D(GL_TEXTURE_2D, static_cast<int32_t>(i), format, w, h, 0,
                                       size, buffer + offset);
                offset += size;
                w /= 2;
                h /= 2;
            }
        }

        //discard any odd mipmaps, ensure a complete texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1);

        EXIT
    }
}
