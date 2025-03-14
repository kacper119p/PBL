//
// Created by Kacper on 28.01.2025.
//

#pragma once

#include "glm/vec3.hpp"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/Components/Interfaces/IUpdateable.h"

namespace Engine
{
    /**
     * @brief Rotates owner.
     */
    class Rotator : public Component, IUpdateable
    {
    private:
        glm::vec3 Velocity;

    public:
        /**
         * @brief Creates a new rotator.
         * @param Velocity Rotation velocity around major axes in degrees per second.
         */
        explicit Rotator(const glm::vec3& Velocity);

    public:
        ~Rotator() override;

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
        void OnAdd(Entity* NewOwner) override;

        void Update(float DeltaTime) override;
    };

} // Engine
