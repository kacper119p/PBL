#pragma once

#include "glm/vec3.hpp"
#include "Engine/Components/Updateable.h"

namespace Engine
{
    /**
     * @brief Simulates ship rolling by applying sine waves to the owner's rotation.
     */
    class ShipRoller final : public Updateable
    {
    private:
        glm::vec3 Amplitude = glm::vec3(0.0f);
        glm::vec3 Velocity = glm::vec3(0.0f);
        glm::vec3 InitialRotation = glm::vec3(0.0f);

    public:
        /**
         * @brief Initializes ShipRoller with default values.
         */
        ShipRoller() = default;

        /**
         * @brief Creates a new ShipRoller instance.
         * @param Amplitude Amplitude of the rotation in degrees.
         * @param Velocity Velocity of the rotation per axis  (Period is 2*Pi).
         */
        ShipRoller(const glm::vec3& Amplitude, const glm::vec3& Velocity);

    public:
        ~ShipRoller() override = default;

    public:
        /**
         * @brief Returns amplitude of the rotation.
         */
        [[nodiscard]] const glm::vec3& GetAmplitude() const
        {
            return Amplitude;
        }

        /**
         * @brief Sets amplitude of the rotation in degrees.
         * @param Amplitude A new amplitude.
         */
        void SetAmplitude(const glm::vec3& Amplitude)
        {
            ShipRoller::Amplitude = Amplitude;
        }

        /**
         * @brief Returns velocity of the rotation per axis (Period is 2*Pi).
         */
        [[nodiscard]] const glm::vec3& GetVelocity() const
        {
            return Velocity;
        }

        /**
         * @brief Sets velocity of the rotation  per axis (Period is 2*Pi).
         * @param Velocity A new velocity.
         */
        void SetVelocity(const glm::vec3& Velocity)
        {
            ShipRoller::Velocity = Velocity;
        }

        /**
         * @brief Returns Initial rotation of the owner.
         */
        [[nodiscard]] const glm::vec3& GetInitialRotation() const
        {
            return InitialRotation;
        }

        /**
         * @brief Sets initial rotation of the owner.
         * @param InitialRotation A new initial rotation.
         */
        void SetInitialRotation(const glm::vec3& InitialRotation)
        {
            ShipRoller::InitialRotation = InitialRotation;
        }

    public:
        void Update(float DeltaTime) override;
        #if EDITOR
        void DrawImGui() override;
        #endif
        SERIALIZATION_EXPORT_CLASS(ShipRoller)
    };

} // Engine
