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
        glm::vec3 Velocity;

    public:
        /**
         * @brief Creates a new rotator.
         * @param Velocity Rotation velocity around major axes in degrees per second.
         */
        explicit Rotator(const glm::vec3& Velocity);

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

        rapidjson::Value Serialize(rapidjson::Document::AllocatorType& Allocator) const override;

        void DeserializeValuePass(const rapidjson::Value& Object,
                                  std::unordered_map<GUID, SerializedObject*>& ReferenceMap) override;

        void DeserializeReferencesPass(const rapidjson::Value& Object,
                                       std::unordered_map<GUID, SerializedObject*>& ReferenceMap) override;
    };

} // Engine
