#pragma once

#include "glm/vec3.hpp"
#include "Engine/Components/Component.h"
#include "Engine/EngineObjects/Entity.h"

namespace Engine
{
    /**
    * @brief Directional light component.
    */
    class DirectionalLight : public Component
    {
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

        void OnAdd(Entity* NewOwner) override;
    };

} // Engine
