#pragma once

#include "Engine/EngineObjects/Entity.h" 
#include "../Component.h"
#include "../Interfaces/IUpdateable.h"
#include "Serialization/SerializationUtility.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace Engine
{
    class Collider;

    class RigidBody : public Component
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
        float mass = 1.0f;
        

        glm::vec3 linearVelocity = glm::vec3(0.0f);
        glm::vec3 angularVelocity = glm::vec3(0.0f);
        glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

        glm::vec3 accumulatedForce = glm::vec3(0.0f);
        glm::vec3 accumulatedTorque = glm::vec3(0.0f);

        float staticFriction = 0.0f;
        float dynamicFriction = 0.0f;
        float restitution = 0.5f;

        float linearDamping = 0.001f;
        float angularDamping = 0.001f;

        bool gravityEnabled = true;
        glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);

        Constraints constraints = Constraints::None;

        glm::mat3 inertiaTensor = glm::mat3(1.0f);
        glm::mat3 inverseInertiaTensor = glm::mat3(1.0f);

    public:
        float inverseMass = 1.0f;
        RigidBody();
        ~RigidBody();

        RigidBody& operator=(const RigidBody& other);

        void OnCollision(const glm::vec3& collisionNormal, const glm::vec3& collisionPoint, float penetrationDepth,
                         RigidBody* otherBody = nullptr);

        void AddForce(const glm::vec3& force, ForceType type = ForceType::Force);
        void AddTorque(const glm::vec3& torque, ForceType type = ForceType::Force);

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

        void EnableGravity(bool enabled);
        bool IsGravityEnabled() const;
        void SetGravity(const glm::vec3& newGravity);
        const glm::vec3& GetGravity() const;

        void Start() override;
        void Update(float deltaTime);

        const glm::vec3& GetLinearVelocity() const;
        const glm::vec3& GetAngularVelocity() const;
        const glm::quat& GetOrientation() const;

        SERIALIZATION_EXPORT_CLASS(RigidBody)

#if EDITOR
        void DrawImGui() override {};
#endif

    private:
        void UpdateInertiaTensor();
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
