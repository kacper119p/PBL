#pragma once

#include "Engine/Components/Component.h"
#include "Engine/EngineObjects/Entity.h"

namespace Engine
{
    /**
     * @brief Point light component.
     */
    class PointLight : public Component
    {
    private:
        glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);

        float LinearFalloff = 0.0f;
        float QuadraticFalloff = 1.0f;
        float Range = 100.0f;

    public:
        /**
         * @brief Construct a new point light.
         */
        PointLight() = default;

    public:
        ~PointLight() override;

    public:
        /**
         * @brief Returns color of this light.
         */
        [[nodiscard]] const glm::vec3& GetColor() const
        {
            return Color;
        }

        /**
         * @brief Sets color of this light.
         * @param Color New color.
         */
        void SetColor(const glm::vec3& Color)
        {
            this->Color = Color;
        }

        /**
         * @brief Returns linear falloff of this light.
         */
        [[nodiscard]] float GetLinearFalloff() const
        {
            return LinearFalloff;
        }

        /**
         * @brief Sets linear falloff of this light.
         * @param LinearFalloff New falloff.
         */
        void SetLinearFalloff(const float LinearFalloff)
        {
            this->LinearFalloff = LinearFalloff;
        }

        /**
         * @brief Returns quadratic falloff of this light.
         */
        [[nodiscard]] float GetQuadraticFalloff() const
        {
            return QuadraticFalloff;
        }

        /**
         * @brief Sets quadratic falloff of this light.
         * @param QuadraticFalloff New falloff.
         */
        void SetQuadraticFalloff(float QuadraticFalloff)
        {
            this->QuadraticFalloff = QuadraticFalloff;
        }

        /**
         * @brief Returns range of this light.
         */
        [[nodiscard]] float GetRange() const
        {
            return Range;
        }

        /**
         * @brief Sets range of this light
         * @param Range New range.
         */
        void SetRange(const float Range)
        {
            PointLight::Range = Range;
        }

        /**
         * @brief Returns position of this light in world space.
         */
        [[nodiscard]] glm::vec3 GetPosition() const
        {
            return GetOwner()->GetTransform()->GetPositionWorldSpace();
        }

        void OnAdd(Entity* NewOwner) override;
    };

} // Engine
