#pragma once

#include "glm/vec3.hpp"
#include "Engine/Components/Component.h"
#include "Engine/EngineObjects/Entity.h"

namespace Engine
{
    /**
    * @brief Directional light component.
    */
    class DirectionalLight final : public Component
    {
    public:
        struct alignas(16) ShaderData
        {
            glm::mat4 LightSpaceTransform;
            glm::vec3 Direction;
            float __padding0{};
            glm::vec3 Color;
            float __padding1{};

            ShaderData(const glm::mat4& LightSpaceTransform, const glm::vec3& Direction, const glm::vec3& Color) :
                LightSpaceTransform(LightSpaceTransform),
                Direction(Direction),
                Color(Color)
            {
            }
        };

    private:
        glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);

    public:
        /**
         * @brief Constructs a new Directional Light.
         */
        DirectionalLight();

    public:
        ~DirectionalLight() override;

    public:
        /**
         * @brief Returns Color of this light.
         */
        [[nodiscard]] const glm::vec3& GetColor() const
        {
            return Color;
        }

        /**
         * @brief Sets color of this light.
         * @param NewColor New Color.
         */
        void SetColor(const glm::vec3& NewColor)
        {
            Color = NewColor;
        }

        /**
         * @brief Returns direction of this light in world space.
         */
        [[nodiscard]] glm::vec3 GetDirection() const
        {
            return glm::normalize(glm::vec3
                    (
                            GetOwner()->GetTransform()->GetLocalToWorldMatrix()
                            * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)
                            ));
        }

        [[nodiscard]] ShaderData GetShaderData(const glm::mat4& DirectionalLightSpaceTransform) const
        {
            return ShaderData(DirectionalLightSpaceTransform, GetDirection(), Color);
        }


        void Start() override;
#if EDITOR
        void DrawImGui() override;
#endif
        SERIALIZATION_EXPORT_CLASS(DirectionalLight);
    };

} // Engine
