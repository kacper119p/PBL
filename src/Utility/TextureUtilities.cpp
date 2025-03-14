#include "stb_image.h"
#include <format>
#include "FileException.h"
#include "TextureUtilities.h"
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "Shaders/Shader.h"
#include "Engine/Rendering/CubeGeometry.h"
#include "Engine/Rendering/ScreenQuad.h"

namespace
{
    void InitializeCubeMap(unsigned int* Cubemap, unsigned int Resolution)
    {
        glGenTextures(1, Cubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, *Cubemap);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                         Resolution, Resolution, 0, GL_RGB, GL_FLOAT, nullptr);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    void InitializeFbo(unsigned int* Fbo, unsigned int* Rbo, unsigned int Resolution)
    {
        glGenFramebuffers(1, Fbo);
        glGenRenderbuffers(1, Rbo);
        glBindFramebuffer(GL_FRAMEBUFFER, *Fbo);
        glBindRenderbuffer(GL_RENDERBUFFER, *Rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Resolution, Resolution);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER, *Rbo);
    }

    void GetCaptureProjection(glm::mat4* projectionMatrix)
    {
        *projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f,
                                             0.1f, 10.0f);
    }

    void GetCaptureViews(glm::mat4* ViewMatrices)
    {
        ViewMatrices[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
                                      glm::vec3(0.0f, -1.0f, 0.0f));
        ViewMatrices[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
                                      glm::vec3(0.0f, -1.0f, 0.0f));
        ViewMatrices[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                                      glm::vec3(0.0f, 0.0f, 1.0f));
        ViewMatrices[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
                                      glm::vec3(0.0f, 0.0f, -1.0f));
        ViewMatrices[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                                      glm::vec3(0.0f, -1.0f, 0.0f));
        ViewMatrices[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                                      glm::vec3(0.0f, -1.0f, 0.0f));
    }

    void InitializeViewPort(unsigned int Resolution)
    {
        glViewport(0, 0, Resolution, Resolution);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }
}

namespace Utility
{
    [[nodiscard]]unsigned int LoadTexture2DFromFile(const char* const FilePath, const GLenum Format,
                                                    const uint8_t SourceChannels, const GLenum SourceFormat)
    {
        int width, height, channelCount;
        GLubyte* data = stbi_load(FilePath, &width, &height,
                                  &channelCount, SourceChannels);

        if (data == nullptr)
        {
            throw Utility::FileException(std::format("Failed to read texture from {}.", FilePath));
        }

        unsigned int textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, Format, width, height, 0, SourceFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        return textureId;
    }

    unsigned int LoadHdrCubeMapFromFile(const char* const FilePath)
    {
        stbi_set_flip_vertically_on_load(true);
        int width, height, channelCount;
        float* data = stbi_loadf(FilePath, &width, &height,
                                 &channelCount, 0);
        stbi_set_flip_vertically_on_load(false);
        if (data == nullptr)
        {
            throw Utility::FileException(std::format("Failed to read texture from {}.", FilePath));
        }

        unsigned int equirectangularTextureId;
        glGenTextures(1, &equirectangularTextureId);
        glBindTexture(GL_TEXTURE_2D, equirectangularTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                     GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        constexpr unsigned int resolution = 1024;

        unsigned int cubemap;
        InitializeCubeMap(&cubemap, resolution);

        glm::mat4 captureProjection;
        GetCaptureProjection(&captureProjection);
        glm::mat4 captureViews[6];
        GetCaptureViews(captureViews);

        Shaders::Shader shader(Shaders::ShaderSourceFiles(
                "./res/shaders/Utility/CubeMapGenerator/CubeMapGenerator.vert",
                nullptr,
                "./res/shaders/Utility/CubeMapGenerator/CubeMapGenerator.frag"));

        shader.Use();
        shader.SetTexture("Texture", 0);
        shader.SetUniform("ProjectionMatrix", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, equirectangularTextureId);

        unsigned int fbo;
        unsigned int rbo;
        InitializeFbo(&fbo, &rbo, resolution);

        Engine::CubeGeometry cube;

        InitializeViewPort(resolution);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        for (unsigned int i = 0; i < 6; ++i)
        {
            shader.SetUniform("ViewMatrix", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            cube.Draw();
        }

        glDeleteTextures(1, &equirectangularTextureId);
        glDeleteFramebuffers(1, &fbo);
        glDeleteRenderbuffers(1, &rbo);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        return cubemap;
    }

    unsigned int IrradianceMapFromEnvironmentMap(const unsigned int EnvironmentMap)
    {
        constexpr unsigned int resolution = 32;

        unsigned int irradianceMap;
        InitializeCubeMap(&irradianceMap, resolution);

        unsigned int fbo;
        unsigned int rbo;
        InitializeFbo(&fbo, &rbo, resolution);

        glm::mat4 captureProjection;
        GetCaptureProjection(&captureProjection);
        glm::mat4 captureViews[6];
        GetCaptureViews(captureViews);

        Shaders::Shader shader(Shaders::ShaderSourceFiles(
                "./res/shaders/Utility/Convolution/Convolution.vert",
                nullptr,
                "./res/shaders/Utility/Convolution/Convolution.frag"));

        shader.Use();
        shader.SetTexture("EnvironmentMap", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, EnvironmentMap);
        shader.SetUniform("ProjectionMatrix", captureProjection);

        Engine::CubeGeometry cube;

        InitializeViewPort(resolution);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        for (unsigned int i = 0; i < 6; ++i)
        {
            shader.SetUniform("ViewMatrix", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            cube.Draw();
        }

        glDeleteFramebuffers(1, &fbo);
        glDeleteRenderbuffers(1, &rbo);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return irradianceMap;
    }

    unsigned int IblPrefilterMapFromEnvironmentMap(unsigned int EnvironmentMap)
    {
        constexpr unsigned int environmentMapResolution = 1024;
        constexpr unsigned int resolution = 1024;
        constexpr unsigned int maxMipLevels = 10;

        unsigned int cubemap;
        InitializeCubeMap(&cubemap, resolution);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        glm::mat4 captureProjection;
        GetCaptureProjection(&captureProjection);
        glm::mat4 captureViews[6];
        GetCaptureViews(captureViews);

        unsigned int fbo;
        unsigned int rbo;
        InitializeFbo(&fbo, &rbo, resolution);

        Engine::CubeGeometry cube;

        Shaders::Shader shader(Shaders::ShaderSourceFiles("./res/shaders/Utility/SpecularIBLSetup/Prefilter.vert",
                                                          nullptr,
                                                          "./res/shaders/Utility/SpecularIBLSetup/Prefilter.frag"));
        shader.Use();
        shader.SetTexture("EnvironmentMap", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, EnvironmentMap);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        shader.SetUniform("ProjectionMatrix", captureProjection);
        shader.SetUniform("Resolution", environmentMapResolution);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        InitializeViewPort(resolution);

        for (unsigned int mipLevel = 0; mipLevel < maxMipLevels; ++mipLevel)
        {
            unsigned int mipWidth = resolution * std::pow(0.5, mipLevel);
            unsigned int mipHeight = resolution * std::pow(0.5, mipLevel);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                                  mipWidth, mipHeight);
            glViewport(0, 0, mipWidth, mipHeight);
            float roughness = static_cast<float>(mipLevel) / static_cast<float>(maxMipLevels - 1);
            shader.SetUniform("Roughness", roughness);
            for (unsigned int i = 0; i < 6; ++i)
            {
                shader.SetUniform("ViewMatrix", captureViews[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                       GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap, mipLevel);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                cube.Draw();
            }
        }

        glDeleteFramebuffers(1, &fbo);
        glDeleteRenderbuffers(1, &rbo);

        glBindTexture(GL_TEXTURE_CUBE_MAP, EnvironmentMap);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return cubemap;
    }

    unsigned int GenerateIblBrdfLut()
    {
        constexpr unsigned int resolution = 1024;

        unsigned int resultTexture;
        glGenTextures(1, &resultTexture);
        glBindTexture(GL_TEXTURE_2D, resultTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 1024, 1024, 0, GL_RG, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        unsigned int fbo;
        unsigned int rbo;
        InitializeFbo(&fbo, &rbo, resolution);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, resultTexture, 0);

        Engine::Rendering::ScreenQuad quad;

        const Shaders::Shader shader(Shaders::ShaderSourceFiles("./res/shaders/Utility/SpecularIBLSetup/BRDF.vert",
                                                          nullptr,
                                                          "./res/shaders/Utility/SpecularIBLSetup/BRDF.frag"));

        InitializeViewPort(resolution);

        shader.Use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        quad.Draw();

        glDeleteFramebuffers(1, &fbo);
        glDeleteRenderbuffers(1, &rbo);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return resultTexture;
    }
} // Utility
