#include "LightManager.h"

#include "Engine/Exceptions/SingletonAlreadyExistsException.h"
#include "glad/glad.h"
#include "CameraRenderData.h"
#include "RenderingManager.h"
#include "Engine/Textures/Texture.h"
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
        InitializeLightBuffer();
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

    void LightManager::SetupLightsForRendering(const CameraRenderData& RenderData)
    {
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, DirectionalLightShadowMap);

        UpdateLightBuffer(RenderData);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, LightBuffer);

        glActiveTexture(GL_TEXTURE12);
        glBindTexture(GL_TEXTURE_CUBE_MAP, PointLightShadowMaps[0]);
        glActiveTexture(GL_TEXTURE13);
        glBindTexture(GL_TEXTURE_CUBE_MAP, PointLightShadowMaps[1]);
        glActiveTexture(GL_TEXTURE14);
        glBindTexture(GL_TEXTURE_CUBE_MAP, SpotLightShadowMaps[0]);
        glActiveTexture(GL_TEXTURE15);
        glBindTexture(GL_TEXTURE_CUBE_MAP, SpotLightShadowMaps[1]);

        glActiveTexture(GL_TEXTURE16);
        glBindTexture(GL_TEXTURE_CUBE_MAP, IrradianceMap);
        glActiveTexture(GL_TEXTURE17);
        glBindTexture(GL_TEXTURE_CUBE_MAP, PrefilterMap);
        glActiveTexture(GL_TEXTURE18);
        glBindTexture(GL_TEXTURE_2D, BrdfLUT);
        glActiveTexture(GL_TEXTURE19);
        glBindTexture(GL_TEXTURE_2D, RenderingManager::GetInstance()->GetSsaoTextureId());
    }

    void LightManager::BindLightScreenPositionBuffer() const
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, LightsScreenSpacePositionsBuffer);
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

    void LightManager::ClearAllLights()
    {
        DirectionalLight = nullptr;
        PointLights.clear();
        SpotLights.clear();
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

    void LightManager::InitializeLightBuffer()
    {
        glGenBuffers(1, &LightBuffer);
        glGenBuffers(1, &LightsScreenSpacePositionsBuffer);
    }

    void LightManager::UpdateLightBuffer(const CameraRenderData& RenderData)
    {
        LightsScreenPositionBuffer.clear();
        LightsScreenPositionBuffer.emplace_back(0.0f); // reserve for light count
        ScreenLightsCount = 0;
        LightBufferData.DirectionalLightCount = DirectionalLight == nullptr ? 0 : 1;
        LightBufferData.PointLightCount = PointLights.size();
        LightBufferData.SpotlightCount = SpotLights.size();

        if (LightBufferData.DirectionalLightCount != 0)
        {
            LightBufferData.DirectionalLight = DirectionalLight->GetShaderData(DirectionalLightSpaceTransform);
            AddLightScreenPosition(RenderData, DirectionalLight);
        }

        if (LightBufferData.PointLightCount > 0)
        {
            LightBufferData.PointLights[0] = PointLights[0]->GetShaderData();
            AddLightScreenPosition(RenderData, PointLights[0]);
        }

        if (LightBufferData.PointLightCount > 1)
        {
            LightBufferData.PointLights[1] = PointLights[1]->GetShaderData();
            AddLightScreenPosition(RenderData, PointLights[1]);
        }

        if (LightBufferData.SpotlightCount > 0)
        {
            LightBufferData.SpotLights[0] = SpotLights[0]->GetShaderData();
            AddLightScreenPosition(RenderData, SpotLights[0]);
        }

        if (LightBufferData.SpotlightCount > 1)
        {
            LightBufferData.SpotLights[1] = SpotLights[1]->GetShaderData();
            AddLightScreenPosition(RenderData, SpotLights[1]);
        }


        for (uint32_t i = 2, skipped = 0; i < LightBufferData.PointLightCount; ++i)
        {
            if (!IsPointLightVisible(PointLights[i]))
            {
                LightBufferData.PointLightCount--;
                skipped++;
                continue;
            }

            LightBufferData.PointLightsDynamic[i - 2 - skipped] = PointLights[i]->GetShaderData();
            AddLightScreenPosition(RenderData, PointLights[i]);
        }

        //Avoids conversion (required to upload correct data)
        LightsScreenPositionBuffer[0].x = *reinterpret_cast<float*>(&ScreenLightsCount);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, LightBufferData.GetCurrentSize(), &LightBufferData, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightsScreenSpacePositionsBuffer);
        glBufferData(
                GL_SHADER_STORAGE_BUFFER,
                static_cast<GLsizeiptr>(LightsScreenPositionBuffer.size() * sizeof(glm::vec2)),
                LightsScreenPositionBuffer.data(), GL_DYNAMIC_DRAW);
    }

    void LightManager::AddLightScreenPosition(const CameraRenderData& RenderData,
                                              const class Engine::DirectionalLight* const Light)
    {
        const glm::vec2 lightPosition = static_cast<glm::vec2>(
                                            static_cast<glm::mat3>(RenderData.ViewMatrix) *
                                            Light->GetDirection()) * 0.5f + 0.5f;
        if (lightPosition.x < -0.5 || lightPosition.x > 1.5f || lightPosition.y < -0.5 || lightPosition.y > 1.5f)
        {
            return;
        }
        LightsScreenPositionBuffer.push_back(lightPosition);
        ScreenLightsCount++;
    }

    void LightManager::AddLightScreenPosition(const CameraRenderData& RenderData, const class PointLight* const Light)
    {
        const glm::vec4 ndc = RenderData.ProjectionMatrix * RenderData.ViewMatrix * glm::vec4(
                                      Light->GetPosition(), 1.0f);
        const glm::vec2 lightPosition = static_cast<glm::vec2>(ndc) / ndc.w * 0.5f + 0.5f;
        if (lightPosition.x < -0.5f || lightPosition.x > 1.5f || lightPosition.y < -0.5 || lightPosition.y > 1.5f)
        {
            return;
        }
        LightsScreenPositionBuffer.push_back(lightPosition);
        ScreenLightsCount++;
    }

    void LightManager::AddLightScreenPosition(const CameraRenderData& RenderData, const class SpotLight* const Light)
    {
        const glm::vec4 ndc = RenderData.ProjectionMatrix * RenderData.ViewMatrix * glm::vec4(
                                      Light->GetPosition(), 1.0f);
        const glm::vec2 lightPosition = static_cast<glm::vec2>(ndc) / ndc.w * 0.5f + 0.5f;
        if (lightPosition.x < -0.5 || lightPosition.x > 1.5f || lightPosition.y < -0.5 || lightPosition.y > 1.5f)
        {
            return;
        }
        LightsScreenPositionBuffer.push_back(lightPosition);
        ScreenLightsCount++;
    }

    bool LightManager::IsPointLightVisible(const PointLight* const Light)
    {
        const Frustum& frustum = RenderingManager::GetInstance()->GetFrustum();
        const glm::vec3& position = Light->GetPosition();
        const float radius = Light->GetRange();
        return frustum.IsSphereVisible(position, radius);
    }

    void LightManager::SetEnvironmentMap(const Texture EnvironmentMap)
    {
        if (IrradianceMap != 0)
        {
            glDeleteTextures(1, &IrradianceMap);
            glDeleteTextures(1, &PrefilterMap);
        }
        IrradianceMap = Utility::IrradianceMapFromEnvironmentMap(EnvironmentMap.GetId());
        PrefilterMap = Utility::IblPrefilterMapFromEnvironmentMap(EnvironmentMap.GetId());
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
