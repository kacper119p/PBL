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


namespace Engine
{
    class LightManager
    {
    public:
        static constexpr unsigned int MaxDirectionalLights = 1;
        static constexpr unsigned int MaxPointLights = 8;
        static constexpr unsigned int MaxSpotLights = 8;
        static constexpr unsigned int MaxDirectionalLightsShadowsSupported = 1;
        static constexpr unsigned int MaxPointLightsShadowsSupported = 2;
        static constexpr unsigned int MaxSpotLightsShadowsSupported = 2;

    private:
        class DirectionalLight* DirectionalLight = nullptr;

        static glm::mat4 DirectionalLightProjectionMatrix;

        glm::mat4 DirectionalLightSpaceTransform;
        unsigned int DirectionalLightShadowMap;
        unsigned int DirectionalLightFrameBuffer;

        std::vector<PointLight*> PointLights;
        glm::mat4 PointLightSpaceTransforms[MaxPointLightsShadowsSupported * 6];
        unsigned int PointLightShadowMaps[MaxPointLightsShadowsSupported];
        unsigned int PointLightFrameBuffers[MaxPointLightsShadowsSupported];

        std::vector<SpotLight*> SpotLights;
        glm::mat4 SpotLightSpaceTransforms[MaxSpotLightsShadowsSupported * 6];
        unsigned int SpotLightShadowMaps[MaxSpotLightsShadowsSupported];
        unsigned int SpotLightFrameBuffers[MaxSpotLightsShadowsSupported];

        unsigned int IrradianceMap = 0;
        unsigned int PrefilterMap = 0;
        unsigned int BrdfLUT;

        static LightManager* Instance;

        static constexpr unsigned int DirectionalShadowWidth = 4096;
        static constexpr unsigned int DirectionalShadowHeight = 4096;
        static constexpr unsigned int OmnidirectionalShadowWidth = 1024;
        static constexpr unsigned int OmnidirectionalShadowHeight = 1024;

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

        void SetEnvironmentMap(unsigned int EnvironmentMap);

        void RenderShadowMaps(const CameraRenderData& RenderData);

        void SetupLightsForRendering(const Shaders::Shader& Shader);

    private:
        void InitializeDirectionalLightShadowMap();

        void InitializeShadowCubeMap(unsigned int& Texture, unsigned int& Framebuffer);

        void RenderDirectionalLightShadowMap(const CameraRenderData& RenderData);

        void RenderPointLightsShadowMaps();

        void RenderSpotLightsShadowMaps();

        void
        RenderOmniDirectionalShadowMap(const unsigned int& Framebuffer, const glm::vec3& LightPosition,
                                       float LightRange, glm::mat4* SpaceTransformMatrices);

        void SetupPointLightsForRendering(const Shaders::Shader& Shader);

        void SetupSpotLightsForRendering(const Shaders::Shader& Shader);
    };

} // Engine
