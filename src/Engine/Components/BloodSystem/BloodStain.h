#pragma once
#include <glm/vec3.hpp>

#include "Engine/Components/Component.h"
#include "Engine/Textures/Texture.h"
#include "Shaders/Shader.h"

#if EDITOR
#include "Engine/Components/Renderers/Renderer.h"
#endif

namespace Engine
{

    class BloodStain final
#if EDITOR
            : public Renderer
#else
            : public Component
#endif
    {
    private:
        Texture Texture;
        glm::vec3 Color = glm::vec3(0.0f, 0.0f, 0.0f);

    public:
        BloodStain();

    public:
        void Draw() const;

        void Start() override;

#if EDITOR
        void RenderDepth(const CameraRenderData& RenderData) override;

        void Render(const CameraRenderData& RenderData) override;

        void RenderDirectionalShadows(const CameraRenderData& RenderData) override;

        void RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                    const glm::mat4* SpaceTransformMatrices) override;

        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_CLASS(BloodStain)
    };

}
