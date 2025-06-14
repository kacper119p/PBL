#pragma once

#include "Engine/Components/Component.h"
#include "Engine/EngineObjects/CameraRenderData.h"

namespace Materials
{
    class Material;
}

namespace Engine
{
    /**
     * @brief Base class for components that render to the screen.
     */
    class Renderer : public Component
    {
    protected:
        Materials::Material* Material = nullptr;

    public:
        ~Renderer() override;

        void Start() override;

        /**
         * @brief Returns Material used by this Renderer.
         */
        [[nodiscard]] Materials::Material* GetMaterial() const
        {
            return Material;
        }

        /**
         * @brief Sets material used by this renderer.
         * @param Material A new material.
         */
        void SetMaterial(Materials::Material* Material);

        /**
         * @brief Renders geometry's depth to a depth buffer. Used for a depth prepass.
         * @param RenderData Properties of the camera used in rendering.
         */
        virtual void RenderDepth(const CameraRenderData& RenderData) = 0;

        /**
         * @brief Renders main pass.
         * @param RenderData Properties of the camera used in rendering.
         */
        virtual void Render(const CameraRenderData& RenderData) = 0;

        /**
         * @brief Renders directional light's shadow map.
         * @param RenderData Properties of the camera used in rendering.
         */
        virtual void RenderDirectionalShadows(const CameraRenderData& RenderData) = 0;

        /**
         * @brief Renders point light's or spotlight's shadow map.
         * @param LightPosition Position of the light in world space.
         * @param LightRange Range of the light.
         * @param SpaceTransformMatrices Cached matrices to transform positions from world space to ndf space of cube faces.
         */
        virtual void
        RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                               const glm::mat4* SpaceTransformMatrices) = 0;
    };
} // Engine
