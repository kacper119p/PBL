#pragma once

#include "Engine/Components/Renderers/Renderer.h"
#include "Materials/Material.h"
#include "Models/Model.h"
#include "Engine/EngineObjects/Camera.h"

namespace Engine
{
    /**
     * @brief Renderer used for rendering meshes.
     */
    class ModelRenderer : public Renderer
    {
    private:
        Materials::Material* Material;
        Models::Model* Model;

    public:
        /**
         * @brief
         * @param Material Material to be used.
         * @param Model Model to be used.
         */
        ModelRenderer(Materials::Material* Material, Models::Model* Model);

    public:
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
        void SetMaterial(Materials::Material* const Material)
        {
            this->Material = Material;
        }

        /**
         * @brief Returns model used by this renderer.
         */
        [[nodiscard]] Models::Model* GetModel() const
        {
            return Model;
        }

        /**
         * @brief Sets model used by this renderer.
         * @param Model A new model.
         */
        void SetModel(Models::Model* const Model)
        {
            this->Model = Model;
        }

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
} // Engine
