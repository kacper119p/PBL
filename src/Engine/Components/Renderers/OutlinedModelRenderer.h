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
    class OutlinedModelRenderer final : public Renderer
    {
    private:
        static Shaders::Shader OutlineShader;
        static int32_t ColorUniformLocation;
        static int32_t OutlineSizeUniformLocation;

    private:
        Models::Model* Model = nullptr;

        glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);
        float OutlineSize = 0.05f;
        bool Active = false;
        bool CastShadow = true;
        bool Culled = false;

    public:
        /**
         * @brief Initializes OutlinedModelRenderer with default values.
         */
        OutlinedModelRenderer() = default;

    public:
        ~OutlinedModelRenderer() override;

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

        /**
         * @brief Makes this renderer render its outline.
         */
        void Activate()
        {
            Active = true;
        }

        /**
         * @brief Stops this renderer from rendering its outline.
         */
        void Deactivate()
        {
            Active = false;
        }

        /**
         * @brief Returns true if this rendering is rendering its outline, false otherwise.
         */
        bool IsActive() const
        {
            return Active;
        }

        void Start() override;

        void RenderDepth(const CameraRenderData& RenderData) override;

        void Render(const CameraRenderData& RenderData) override;

        void RenderDirectionalShadows(const CameraRenderData& RenderData) override;

        void RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                    const glm::mat4* SpaceTransformMatrices) override;

        void RenderOutline(const CameraRenderData& RenderData) const;

        static void InitializeShaders();

        static void BindForRendering();

    private:
        void SetupMatrices(const CameraRenderData& RenderData, const Shaders::Shader& Shader) const;

        void Draw() const;
#if EDITOR
        void DrawImGui() override;
#endif
        SERIALIZATION_EXPORT_CLASS(OutlinedModelRenderer)
    };
} // Engine
