#pragma once

#include "../Component.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace Engine
{

    class RigidBody : public Component, public IUpdateable
    {
    public:

        enum class Constraints : uint8_t
        {
            None = 0,
            LockPositionX = 1 << 0,
            LockPositionY = 1 << 1,
            LockPositionZ = 1 << 2,
            LockRotationX = 1 << 3,
            LockRotationY = 1 << 4,
            LockRotationZ = 1 << 5
        };

    private:
        float mass;
        float inverseMass;
        glm::vec3 linearVelocity;
        glm::vec3 angularVelocity;
        glm::quat orientation;

        glm::vec3 accumulatedForce;
        glm::vec3 accumulatedTorque;

        float staticFriction;
        float dynamicFriction;
        float restitution;
        float linearDamping;
        float angularDamping;

        Constraints constraints;

    public:
        RigidBody();

        void AddForce(const glm::vec3& force);
        void AddTorque(const glm::vec3& torque);

        void SetMass(float m);
        void SetInverseMass(float invM);

        void SetFriction(float muStatic, float muDynamic);
        void SetRestitution(float e);

        void SetLinearDamping(float d);
        void SetAngularDamping(float d);

        void SetConstraints(Constraints newConstraints);
        void AddConstraint(Constraints constraint);
        void RemoveConstraint(Constraints constraint);
        bool HasConstraint(Constraints constraint) const;

        void Update(float deltaTime) override;

        const glm::vec3& GetLinearVelocity() const;
        const glm::vec3& GetAngularVelocity() const;
        const glm::quat& GetOrientation() const;
    };

    inline RigidBody::Constraints operator|(RigidBody::Constraints a, RigidBody::Constraints b)
    {
        return static_cast<RigidBody::Constraints>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }

    inline RigidBody::Constraints& operator|=(RigidBody::Constraints& a, RigidBody::Constraints b)
    {
        a = a | b;
        return a;
    }

    inline RigidBody::Constraints operator&(RigidBody::Constraints a, RigidBody::Constraints b)
    {
        return static_cast<RigidBody::Constraints>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
    }

    inline RigidBody::Constraints operator~(RigidBody::Constraints a)
    {
        return static_cast<RigidBody::Constraints>(~static_cast<uint8_t>(a));
    }

} // namespace Engine
