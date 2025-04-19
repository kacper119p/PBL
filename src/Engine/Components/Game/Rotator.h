#pragma once

#include "Engine/Components/Updateable.h"
#include "glm/vec3.hpp"
#include "Engine/EngineObjects/Entity.h"

namespace Engine
{
    /**
     * @brief Rotates owner.
     */
    class Rotator final : public Updateable
    {
    private:
        glm::vec3 Velocity = glm::vec3(0.0f);

    public:
        /**
         * @brief Initializes Rotator with default values.
         */
        Rotator() = default;

    public:
        ~Rotator() override = default;

    public:
        /**
         * @brief Returns rotation velocity around major axes in degrees per second.
         */
        [[nodiscard]] inline const glm::vec3& GetVelocity() const
        {
            return Velocity;
        }

        /**
         * @brief Sets rotation velocity around major axes in degrees per second.
         * @param Velocity A new rotation velocity.
         */
        inline void SetVelocity(const glm::vec3& Velocity)
        {
            Rotator::Velocity = Velocity;
        }

    public:
        void Update(float DeltaTime) override;

        void DrawImGui() override;

        SERIALIZATION_EXPORT_CLASS(Rotator)
    };

} // Engine
