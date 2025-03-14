#pragma once

#include "Engine/Components/Component.h"
#include "Engine/EngineObjects/Entity.h"
#include "glm/glm.hpp"

namespace Engine
{

    class SpotLight : public Component
    {
    private:
        glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);

        float OuterAngle = 90.0f;
        float InnerAngle = 45.0f;

        float LinearFalloff = 0.0f;
        float QuadraticFalloff = 1.0f;
        float Range = 100.0f;

    public:
        ~SpotLight() override;

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
         * @param Color A new color.
         */
        void SetColor(const glm::vec3& Color)
        {
            this->Color = Color;
        }

        /**
         * @brief Returns outer angle of this spotlight in degrees.
         */
        [[nodiscard]] float GetOuterAngle() const
        {
            return OuterAngle;
        }

        /**
         * @brief Sets outer angle of this spotlight in degrees.
         * @param OuterAngle A new angle.
         */
        void SetOuterAngle(const float OuterAngle)
        {
            SpotLight::OuterAngle = OuterAngle;
        }

        /**
         * @brief Returns inner angle of this spotlight in degrees.
         */
        [[nodiscard]] float GetInnerAngle() const
        {
            return InnerAngle;
        }

        /**
         * @brief Sets inner angle of this spotlight in degrees.
         * @param InnerAngle A new angle.
         */
        void SetInnerAngle(const float InnerAngle)
        {
            SpotLight::InnerAngle = InnerAngle;
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
            this->Range = Range;
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
