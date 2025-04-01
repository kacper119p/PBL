#pragma once

#include "Engine/Components/Renderers/Renderer.h"
#include "Engine/EngineObjects/Camera.h"
#include "Materials/Material.h"
#include "Models/ModelAnimated.h"
#include "models/Animation.h"
#include "models/Animator.h"

namespace Engine
{
    /**
     * @brief Renderer used for rendering meshes.
     */
    class AnimatedModelRenderer : public Renderer
    {
    private:
        Materials::Material* Material;
        Models::ModelAnimated* Model;
        Models::Animation* Animation;
        Models::Animator* Animator;

        float deltaTime = 0.0f;
        float lastFrame = 0.0f;

    public:
        /**
         * @brief
         * @param Material Material to be used.
         * @param Model Model to be used.
         */
        AnimatedModelRenderer(Materials::Material* Material, Models::ModelAnimated* Model, Models::Animation* Animation,
                              Models::Animator* Animator);

    public:
        /**
         * @brief Returns Material used by this Renderer.
         */
        [[nodiscard]] Materials::Material* GetMaterial() const { return Material; }

        /**
         * @brief Sets material used by this renderer.
         * @param Material A new material.
         */
        void SetMaterial(Materials::Material* const Material) { this->Material = Material; }

        /**
         * @brief Returns model used by this renderer.
         */
        [[nodiscard]] Models::ModelAnimated* GetModel() const { return Model; }

        /**
         * @brief Sets model used by this renderer.
         * @param Model A new model.
         */
        void SetModel(Models::ModelAnimated* const Model) { this->Model = Model; }

    public:
        void RenderDepth(const CameraRenderData& RenderData) override;

        void Render(const CameraRenderData& RenderData) override;

        void RenderDirectionalShadows(const CameraRenderData& RenderData) override;

        void RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                    const glm::mat4* SpaceTransformMatrices) override;

    private:
        void SetupMatrices(const CameraRenderData& RenderData, const Shaders::Shader& Shader) const;

        void Draw() const;
    };
} // namespace Engine
