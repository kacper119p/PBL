#pragma once

#include "Engine/Components/Lights/DirectionalLight.h"
#include "Engine/Components/Lights/PointLight.h"
#include "Engine/Components/Lights/SpotLight.h"

namespace Engine
{
    /**
     * @brief Used for ImGui light properties manipulation.
     */
    class LightsGui
    {
    private:
        static std::vector<DirectionalLight*> DirectionalLights;
        static std::vector<PointLight*> PointLights;
        static std::vector<SpotLight*> SpotLights;

        static Component* Current;
        static std::string CurrentName;

    public:
        virtual ~LightsGui() = 0;

    public:
        inline static void RegisterLight(class DirectionalLight* NewLight)
        {
            if (std::find(DirectionalLights.begin(), DirectionalLights.end(), NewLight) == DirectionalLights.end())
            {
                DirectionalLights.push_back(NewLight);
            }
        }

        inline static void UnregisterLight(class DirectionalLight* LightToUnregister)
        {
            std::erase(DirectionalLights, LightToUnregister);
        }

        inline static void RegisterLight(class PointLight* NewLight)
        {
            if (std::find(PointLights.begin(), PointLights.end(), NewLight) == PointLights.end())
            {
                PointLights.push_back(NewLight);
            }
        }

        inline static void UnregisterLight(class PointLight* LightToUnregister)
        {
            std::erase(PointLights, LightToUnregister);
        }

        inline static void RegisterLight(class SpotLight* NewLight)
        {
            if (std::find(SpotLights.begin(), SpotLights.end(), NewLight) == SpotLights.end())
            {
                SpotLights.push_back(NewLight);
            }
        }

        inline static void UnregisterLight(class SpotLight* LightToUnregister)
        {
            std::erase(SpotLights, LightToUnregister);
        }

    public:
        static void Draw();

    private:
        static void LightGui(DirectionalLight* Light);

        static void LightGui(PointLight* Light);

        static void LightGui(SpotLight* Light);

        static bool ManageColor(glm::vec3& Color);
    };
} // Engine
