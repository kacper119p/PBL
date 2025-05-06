#pragma once

#include "../Component.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "Serialization/SerializationUtility.h"
#include "../../EngineObjects/Entity.h" // TODO: Fix later. I'm using this way because of indexing problem.
#include "../Interfaces/IUpdateable.h"
#include "../../EngineObjects/UpdateManager.h"


namespace Engine
{
    class Collider;
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

        enum class ForceType
        {
            Force,
            Impulse 
        };

    private:

        // TODO: remove when scriptable fully implemented
        float timeSinceLastForce = 0.0f;

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
        ~RigidBody();

        RigidBody& operator=(const RigidBody& other);
        
        void OnCollision(const glm::vec3& collisionNormal, const glm::vec3& collisionPoint, float penetrationDepth);

        void AddForce(const glm::vec3& force, ForceType type = ForceType::Force);
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

        void Start() override;
        void Update(float deltaTime) override;

        const glm::vec3& GetLinearVelocity() const;
        const glm::vec3& GetAngularVelocity() const;
        const glm::quat& GetOrientation() const;

        SERIALIZATION_EXPORT_CLASS(RigidBody)
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

    inline RigidBody::Constraints& operator&=(RigidBody::Constraints& a, RigidBody::Constraints b)
    {
        a = a & b;
        return a;
    }

    inline RigidBody::Constraints operator~(RigidBody::Constraints a)
    {
        return static_cast<RigidBody::Constraints>(~static_cast<uint8_t>(a));
    }

} // namespace Engine
