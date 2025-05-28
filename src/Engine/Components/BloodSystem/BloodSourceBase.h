#pragma once
#include <glm/vec3.hpp>

#include "Engine/Textures/Texture.h"

#if EDITOR
#include "Engine/Components/Renderers/Renderer.h"
#else
#include "Engine/Components/Component.h"
#endif

namespace Engine
{

    class BloodSourceBase
#if EDITOR
            : public Renderer
#else
            : public Component
#endif
    {
    protected:
        Texture Texture;
        glm::vec3 Color = glm::vec3(0.0f, 0.0f, 0.0f);

    public:
        BloodSourceBase();

    public:
        [[nodiscard]] Engine::Texture GetTexture() const
        {
            return Texture;
        }

        void SetTexture(const Engine::Texture& Texture)
        {
            this->Texture = Texture;
        }

        [[nodiscard]] glm::vec3 GetColor() const
        {
            return Color;
        }

        void SetColor(const glm::vec3& Color)
        {
            this->Color = Color;
        }

        void Start() override;

        void Draw() const; // Should not be virtual
#if EDITOR

    public:
        void RenderDepth(const CameraRenderData& RenderData) override;

        void Render(const CameraRenderData& RenderData) override;

        void RenderDirectionalShadows(const CameraRenderData& RenderData) override;

        void RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                    const glm::mat4* SpaceTransformMatrices) override;

        void DrawImGui() override;
#endif
    };

}
