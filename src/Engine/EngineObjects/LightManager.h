#pragma once

#include <format>
#include <vector>
#include "Shaders/Shader.h"
#include "Engine/Exceptions/LightCapacityReachedException.h"
#include "Engine/Components/Lights/DirectionalLight.h"
#include "Engine/Components/Lights/PointLight.h"
#include "Camera.h"
#include "CameraRenderData.h"
#include "Engine/Components/Lights/SpotLight.h"
#include "Engine/Textures/Texture.h"


namespace Engine
{
    class LightManager
    {
    public:
        static constexpr uint8_t MaxDirectionalLights = 1;
        static constexpr uint8_t MaxPointLights = 8;
        static constexpr uint8_t MaxSpotLights = 2;
        static constexpr uint8_t MaxDirectionalLightsShadowsSupported = 1;
        static constexpr uint8_t MaxPointLightsShadowsSupported = 2;
        static constexpr uint8_t MaxSpotLightsShadowsSupported = 2;

    private:
        struct alignas(16) LightDataBuffer
        {
            uint32_t DirectionalLightCount = 0;
            uint32_t PointLightCount = 0;
            uint32_t SpotlightCount = 0;
            float _padding0{};
            DirectionalLight::ShaderData DirectionalLight{};
            PointLight::ShaderData PointLights[2]{};
            SpotLight::ShaderData SpotLights[2]{};
            PointLight::ShaderData PointLightsDynamic[MaxPointLights - 2]{};

            LightDataBuffer()
            {
            }

            [[nodiscard]] int64_t GetCurrentSize() const
            {
                return sizeof(LightDataBuffer)
                       - (MaxPointLights - PointLightCount - 2) * sizeof(PointLight::ShaderData);
            }
        };

    private:
        class DirectionalLight* DirectionalLight = nullptr;

        static glm::mat4 DirectionalLightProjectionMatrix;

        glm::mat4 DirectionalLightSpaceTransform;
        uint32_t DirectionalLightShadowMap;
        uint32_t DirectionalLightFrameBuffer;

        std::vector<PointLight*> PointLights;
        glm::mat4 PointLightSpaceTransforms[MaxPointLightsShadowsSupported * 6];
        uint32_t PointLightShadowMaps[MaxPointLightsShadowsSupported];
        uint32_t PointLightFrameBuffers[MaxPointLightsShadowsSupported];

        std::vector<SpotLight*> SpotLights;
        glm::mat4 SpotLightSpaceTransforms[MaxSpotLightsShadowsSupported * 6];
        uint32_t SpotLightShadowMaps[MaxSpotLightsShadowsSupported];
        uint32_t SpotLightFrameBuffers[MaxSpotLightsShadowsSupported];

        uint32_t IrradianceMap = 0;
        uint32_t PrefilterMap = 0;
        uint32_t BrdfLUT;

        uint32_t SsaoTexture = 0;

        uint32_t LightBuffer;
        LightDataBuffer LightBufferData;
        uint32_t LightsScreenSpacePositionsBuffer;
        std::vector<glm::vec2> LightsScreenPositionBuffer;

        static LightManager* Instance;

        static constexpr uint32_t DirectionalShadowWidth = 4096;
        static constexpr uint32_t DirectionalShadowHeight = 4096;
        static constexpr uint32_t OmnidirectionalShadowWidth = 1024;
        static constexpr uint32_t OmnidirectionalShadowHeight = 1024;

    private:
        LightManager();

    public:
        virtual ~LightManager();

    public:
        static void Initialize();

    public:
        LightManager(LightManager const&) = delete;

        void operator=(LightManager const&) = delete;

    public:
        static LightManager* GetInstance()
        {
            return Instance;
        }

        void RegisterLight(class DirectionalLight* NewLight)
        {
            if (DirectionalLight)
            {
                throw LightCapacityReachedException(
                        std::format("Reached directional lights capacity of {}.", MaxDirectionalLights));
            }
            DirectionalLight = NewLight;
        }

        void UnregisterLight(class DirectionalLight* LightToUnregister)
        {
            if (DirectionalLight == LightToUnregister)
            {
                DirectionalLight = nullptr;
            }
        }

        void ClearAllLights();

        void RegisterLight(class PointLight* NewLight)
        {
            if (PointLights.size() >= MaxPointLights)
            {
                throw LightCapacityReachedException(
                        std::format("Reached point lights capacity of {}.", MaxPointLights));
            }
            if (std::find(PointLights.begin(), PointLights.end(), NewLight) == PointLights.end())
            {
                PointLights.push_back(NewLight);
            }
        }

        void UnregisterLight(class PointLight* LightToUnregister)
        {
            std::erase(PointLights, LightToUnregister);
        }

        void RegisterLight(class SpotLight* NewLight)
        {
            if (SpotLights.size() >= MaxSpotLights)
            {
                throw LightCapacityReachedException(
                        std::format("Reached point lights capacity of {}.", MaxSpotLights));
            }
            if (std::find(SpotLights.begin(), SpotLights.end(), NewLight) == SpotLights.end())
            {
                SpotLights.push_back(NewLight);
            }
        }

        void UnregisterLight(class SpotLight* LightToUnregister)
        {
            std::erase(SpotLights, LightToUnregister);
        }

        void SetEnvironmentMap(Texture EnvironmentMap);

        void RenderShadowMaps(const CameraRenderData& RenderData);

        void SetupLightsForRendering(const CameraRenderData& RenderData);

        void BindLightScreenPositionBuffer() const;

    private:
        void InitializeDirectionalLightShadowMap();

        void InitializeShadowCubeMap(unsigned int& Texture, unsigned int& Framebuffer);

        void RenderDirectionalLightShadowMap(const CameraRenderData& RenderData);

        void RenderPointLightsShadowMaps();

        void RenderSpotLightsShadowMaps();

        void InitializeLightBuffer();

        void UpdateLightBuffer(const CameraRenderData& RenderData);

        void AddLightScreenPosition(const CameraRenderData& RenderData, const class DirectionalLight* Light);

        void AddLightScreenPosition(const CameraRenderData& RenderData, const class PointLight* Light);

        void AddLightScreenPosition(const CameraRenderData& RenderData, const class SpotLight* Light);

        void
        RenderOmniDirectionalShadowMap(const unsigned int& Framebuffer, const glm::vec3& LightPosition,
                                       float LightRange, glm::mat4* SpaceTransformMatrices);
    };

} // Engine
