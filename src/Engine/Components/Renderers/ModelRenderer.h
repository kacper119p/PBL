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
    class ModelRenderer final : public Renderer
    {
    private:
        Models::Model* Model = nullptr;
        bool CastShadow = true;

        bool Culled = false;

    public:
        /**
         * @brief Initializes ModelRenderer with default values.
         */
        ModelRenderer() = default;

    public:
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

        /**
         * @brief Returns true if this object is a shadow caster.
         */
        [[nodiscard]] bool CastsShadow() const
        {
            return CastShadow;
        }

        /**
         * @brief Should this object cast shadows?
         * @param CastShadow Set to true if object should be a shadow caster, false otherwise.
         */
        void SetCastShadow(const bool CastShadow)
        {
            this->CastShadow = CastShadow;
        }

        void RenderDepth(const CameraRenderData& RenderData) override;

        void Render(const CameraRenderData& RenderData) override;

        void RenderDirectionalShadows(const CameraRenderData& RenderData) override;

        void RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                    const glm::mat4* SpaceTransformMatrices) override;

    private:
        void SetupMatrices(const CameraRenderData& RenderData, const Shaders::Shader& Shader) const;

        void Draw() const;
#if EDITOR
        void DrawImGui() override;
#endif
        SERIALIZATION_EXPORT_CLASS(ModelRenderer)
    };
} // Engine
