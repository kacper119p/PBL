#include "LightManager.h"

#include "Engine/Exceptions/SingletonAlreadyExistsException.h"
#include "glad/glad.h"
#include "CameraRenderData.h"
#include "RenderingManager.h"
#include "Utility/TextureUtilities.h"

namespace Engine
{
    glm::mat4 LightManager::DirectionalLightProjectionMatrix
            = glm::ortho(-60.0f, 60.0f, -60.0f, 60.0f,
                         0.1f, 300.0f);

    LightManager* LightManager::Instance = nullptr;

    LightManager::LightManager() :
        BrdfLUT(Utility::GenerateIblBrdfLut())
    {
        InitializeDirectionalLightShadowMap();
        InitializeShadowCubeMap(PointLightShadowMaps[0], PointLightFrameBuffers[0]);
        InitializeShadowCubeMap(PointLightShadowMaps[1], PointLightFrameBuffers[1]);
        InitializeShadowCubeMap(SpotLightShadowMaps[0], SpotLightFrameBuffers[0]);
        InitializeShadowCubeMap(SpotLightShadowMaps[1], SpotLightFrameBuffers[1]);
    }

    void LightManager::RenderShadowMaps(const CameraRenderData& RenderData)
    {
        RenderDirectionalLightShadowMap(RenderData);
        RenderPointLightsShadowMaps();
        RenderSpotLightsShadowMaps();
    }

    void LightManager::SetupLightsForRendering(const Shaders::Shader& Shader)
    {
        if (DirectionalLight)
        {
            Shader.SetUniform("DirectionalLight.Direction", DirectionalLight->GetDirection());
            Shader.SetUniform("DirectionalLight.Color", DirectionalLight->GetColor());
            Shader.Shader::SetUniform("DirectionalLightSpaceTransform", DirectionalLightSpaceTransform);
            glActiveTexture(GL_TEXTURE11);
            glBindTexture(GL_TEXTURE_2D, DirectionalLightShadowMap);
        }

        SetupPointLightsForRendering(Shader);
        SetupSpotLightsForRendering(Shader);

        glActiveTexture(GL_TEXTURE16);
        glBindTexture(GL_TEXTURE_CUBE_MAP, IrradianceMap);
        glActiveTexture(GL_TEXTURE17);
        glBindTexture(GL_TEXTURE_CUBE_MAP, PrefilterMap);
        glActiveTexture(GL_TEXTURE18);
        glBindTexture(GL_TEXTURE_2D, BrdfLUT);
    }

    void LightManager::SetupPointLightsForRendering(const Shaders::Shader& Shader)
    {
        Shader.SetUniform("PointLightsCount", static_cast<GLuint>(PointLights.size()));

        if (PointLights.empty())
        {
            return;
        }

        Shader.SetUniform("PointLights[0].Position", PointLights[0]->GetPosition());
        Shader.SetUniform("PointLights[0].Color", PointLights[0]->GetColor());
        Shader.SetUniform("PointLights[0].LinearFalloff", PointLights[0]->GetLinearFalloff());
        Shader.SetUniform("PointLights[0].QuadraticFalloff", PointLights[0]->GetQuadraticFalloff());
        Shader.SetUniform("PointLights[0].Range", PointLights[0]->GetRange());
        glActiveTexture(GL_TEXTURE12);
        glBindTexture(GL_TEXTURE_CUBE_MAP, PointLightShadowMaps[0]);

        if (PointLights.size() < 2)
        {
            return;
        }
        Shader.SetUniform("PointLights[1].Position", PointLights[1]->GetPosition());
        Shader.SetUniform("PointLights[1].Color", PointLights[1]->GetColor());
        Shader.SetUniform("PointLights[1].LinearFalloff", PointLights[1]->GetLinearFalloff());
        Shader.SetUniform("PointLights[1].QuadraticFalloff", PointLights[1]->GetQuadraticFalloff());
        Shader.SetUniform("PointLights[1].Range", PointLights[1]->GetRange());
        glActiveTexture(GL_TEXTURE13);
        glBindTexture(GL_TEXTURE_CUBE_MAP, PointLightShadowMaps[1]);

        for (int i = 2; i < PointLights.size(); ++i)
        {

            Shader.SetUniform(std::format("PointLights[{}].Position", i).c_str(),
                              PointLights[i]->GetPosition());

            Shader.SetUniform(std::format("PointLights[{}].Color", i).c_str(),
                              PointLights[i]->GetColor());

            Shader.SetUniform(std::format("PointLights[{}].LinearFalloff", i).c_str(),
                              PointLights[i]->GetLinearFalloff());

            Shader.SetUniform(std::format("PointLights[{}].QuadraticFalloff", i).c_str(),
                              PointLights[i]->GetQuadraticFalloff());
            Shader.SetUniform(std::format("PointLights[{}].Range", i).c_str(),
                              PointLights[i]->GetRange());
        }
    }

    void LightManager::SetupSpotLightsForRendering(const Shaders::Shader& Shader)
    {
        Shader.SetUniform("SpotLightsCount", static_cast<GLuint>(SpotLights.size()));

        if (SpotLights.empty())
        {
            return;
        }
        Shader.SetUniform("SpotLights[0].Position", SpotLights[0]->GetPosition());
        Shader.SetUniform("SpotLights[0].Direction", SpotLights[0]->GetDirection());
        Shader.SetUniform("SpotLights[0].Color", SpotLights[0]->GetColor());
        Shader.SetUniform("SpotLights[0].OuterAngle",
                          glm::cos(glm::radians(SpotLights[0]->GetOuterAngle())));
        Shader.SetUniform("SpotLights[0].InnerAngle",
                          glm::cos(glm::radians(SpotLights[0]->GetInnerAngle())));
        Shader.SetUniform("SpotLights[0].LinearFalloff", SpotLights[0]->GetLinearFalloff());
        Shader.SetUniform("SpotLights[0].QuadraticFalloff", SpotLights[0]->GetQuadraticFalloff());
        Shader.SetUniform("SpotLights[0].Range", SpotLights[0]->GetRange());
        glActiveTexture(GL_TEXTURE14);
        glBindTexture(GL_TEXTURE_CUBE_MAP, SpotLightShadowMaps[0]);

        if (SpotLights.size() < 2)
        {
            return;
        }
        Shader.SetUniform("SpotLights[1].Position", SpotLights[1]->GetPosition());
        Shader.SetUniform("SpotLights[1].Direction", SpotLights[1]->GetDirection());
        Shader.SetUniform("SpotLights[1].Color", SpotLights[1]->GetColor());
        Shader.SetUniform("SpotLights[1].OuterAngle",
                          glm::cos(glm::radians(SpotLights[1]->GetOuterAngle())));
        Shader.SetUniform("SpotLights[1].InnerAngle",
                          glm::cos(glm::radians(SpotLights[1]->GetInnerAngle())));
        Shader.SetUniform("SpotLights[1].LinearFalloff", SpotLights[1]->GetLinearFalloff());
        Shader.SetUniform("SpotLights[1].QuadraticFalloff", SpotLights[1]->GetQuadraticFalloff());
        Shader.SetUniform("SpotLights[1].Range", SpotLights[1]->GetRange());
        glActiveTexture(GL_TEXTURE15);
        glBindTexture(GL_TEXTURE_CUBE_MAP, SpotLightShadowMaps[1]);

        for (int i = 2; i < SpotLights.size(); ++i)
        {

            Shader.SetUniform(std::format("PointLights[{}].Position", i).c_str(),
                              SpotLights[i]->GetPosition());

            Shader.SetUniform(std::format("PointLights[{}].Direction", i).c_str(),
                              SpotLights[i]->GetDirection());

            Shader.SetUniform(std::format("PointLights[{}].Color", i).c_str(),
                              SpotLights[i]->GetColor());

            Shader.SetUniform(std::format("PointLights[{}].OuterAngle", i).c_str(),
                              glm::cos(SpotLights[i]->GetOuterAngle()));

            Shader.SetUniform(std::format("PointLights[{}].InnerAngle", i).c_str(),
                              glm::cos(glm::radians(SpotLights[i]->GetInnerAngle())));

            Shader.SetUniform(std::format("PointLights[{}].LinearFalloff", i).c_str(),
                              glm::radians(PointLights[i]->GetLinearFalloff()));

            Shader.SetUniform(std::format("PointLights[{}].QuadraticFalloff", i).c_str(),
                              SpotLights[i]->GetQuadraticFalloff());
            Shader.SetUniform(std::format("PointLights[{}].Range", i).c_str(),
                              SpotLights[i]->GetRange());
        }
    }

    void LightManager::InitializeDirectionalLightShadowMap()
    {
        glGenTextures(1, &DirectionalLightShadowMap);
        glBindTexture(GL_TEXTURE_2D, DirectionalLightShadowMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DirectionalShadowWidth,
                     DirectionalShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
        float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glGenFramebuffers(1, &DirectionalLightFrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, DirectionalLightFrameBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                               DirectionalLightShadowMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void LightManager::InitializeShadowCubeMap(unsigned int& Texture, unsigned int& Framebuffer)
    {
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, Texture);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                         OmnidirectionalShadowWidth, OmnidirectionalShadowHeight, 0, GL_DEPTH_COMPONENT,
                         GL_FLOAT, NULL);
        }


        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

        glGenFramebuffers(1, &Framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, Texture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void LightManager::RenderDirectionalLightShadowMap(const CameraRenderData& RenderData)
    {
        if (!DirectionalLight)
        {
            return;
        }
        glViewport(0, 0, DirectionalShadowWidth, DirectionalShadowHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, DirectionalLightFrameBuffer);
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::mat4 lightView = glm::lookAt(RenderData.CameraPosition + DirectionalLight->GetDirection() * 100.0f,
                                          RenderData.CameraPosition,
                                          glm::vec3(0.0f, 1.0f, 0.0f));

        class CameraRenderData renderData(RenderData.CameraPosition, lightView, DirectionalLightProjectionMatrix);

        RenderingManager::GetInstance()->RenderAllDirectionalShadowMap(renderData, DirectionalLightFrameBuffer,
                                                                       DirectionalShadowWidth, DirectionalShadowHeight);
        DirectionalLightSpaceTransform = DirectionalLightProjectionMatrix * lightView;
    }

    void LightManager::RenderOmniDirectionalShadowMap(const unsigned int& Framebuffer,
                                                      const glm::vec3& LightPosition, float LightRange,
                                                      glm::mat4* SpaceTransformMatrices)
    {
        const glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f),
                                                            static_cast<float>(OmnidirectionalShadowWidth) /
                                                            static_cast<float>(OmnidirectionalShadowHeight),
                                                            0.0f,
                                                            LightRange);

        SpaceTransformMatrices[0] = projectionMatrix *
                                    glm::lookAt(LightPosition, LightPosition + glm::vec3(1.0, 0.0, 0.0),
                                                glm::vec3(0.0, -1.0, 0.0));
        SpaceTransformMatrices[1] = projectionMatrix *
                                    glm::lookAt(LightPosition, LightPosition + glm::vec3(-1.0, 0.0, 0.0),
                                                glm::vec3(0.0, -1.0, 0.0));
        SpaceTransformMatrices[2] = projectionMatrix *
                                    glm::lookAt(LightPosition, LightPosition + glm::vec3(0.0, 1.0, 0.0),
                                                glm::vec3(0.0, 0.0, 1.0));
        SpaceTransformMatrices[3] = projectionMatrix *
                                    glm::lookAt(LightPosition, LightPosition + glm::vec3(0.0, -1.0, 0.0),
                                                glm::vec3(0.0, 0.0, -1.0));
        SpaceTransformMatrices[4] = projectionMatrix *
                                    glm::lookAt(LightPosition, LightPosition + glm::vec3(0.0, 0.0, 1.0),
                                                glm::vec3(0.0, -1.0, 0.0));
        SpaceTransformMatrices[5] = projectionMatrix *
                                    glm::lookAt(LightPosition, LightPosition + glm::vec3(0.0, 0.0, -1.0),
                                                glm::vec3(0.0, -1.0, 0.0));

        RenderingManager::GetInstance()->RenderAllPointSpotShadowMap(LightPosition, LightRange,
                                                                     SpaceTransformMatrices,
                                                                     Framebuffer, OmnidirectionalShadowWidth,
                                                                     OmnidirectionalShadowHeight);

    }

    void LightManager::Initialize()
    {
        if (Instance)
        {
            throw SingletonAlreadyExistsException("Instance of LightManager already exists.");
        }
        Instance = new LightManager();
    }

    void LightManager::RenderPointLightsShadowMaps()
    {
        if (PointLights.empty())
        {
            return;
        }
        LightManager::RenderOmniDirectionalShadowMap(PointLightFrameBuffers[0],
                                                     PointLights[0]->GetPosition(),
                                                     PointLights[0]->GetRange(),
                                                     PointLightSpaceTransforms);
        if (PointLights.size() < 2)
        {
            return;
        }
        LightManager::RenderOmniDirectionalShadowMap(PointLightFrameBuffers[1],
                                                     PointLights[1]->GetPosition(),
                                                     PointLights[1]->GetRange(),
                                                     PointLightSpaceTransforms + 6);
    }

    void LightManager::RenderSpotLightsShadowMaps()
    {
        if (SpotLights.empty())
        {
            return;
        }
        LightManager::RenderOmniDirectionalShadowMap(SpotLightFrameBuffers[0],
                                                     SpotLights[0]->GetPosition(),
                                                     SpotLights[0]->GetRange(),
                                                     SpotLightSpaceTransforms);
        if (SpotLights.size() < 2)
        {
            return;
        }
        LightManager::RenderOmniDirectionalShadowMap(SpotLightFrameBuffers[1],
                                                     SpotLights[1]->GetPosition(),
                                                     SpotLights[1]->GetRange(),
                                                     SpotLightSpaceTransforms + 6);
    }

    void LightManager::SetEnvironmentMap(unsigned int EnvironmentMap)
    {
        if (IrradianceMap != 0)
        {
            glDeleteTextures(1, &IrradianceMap);
            glDeleteTextures(1, &PrefilterMap);
        }
        IrradianceMap = Utility::IrradianceMapFromEnvironmentMap(EnvironmentMap);
        PrefilterMap = Utility::IblPrefilterMapFromEnvironmentMap(EnvironmentMap);
    }

    LightManager::~LightManager()
    {
        if (IrradianceMap != 0)
        {
            glDeleteTextures(1, &IrradianceMap);
            glDeleteTextures(1, &PrefilterMap);
        }
    }
} // Engine
