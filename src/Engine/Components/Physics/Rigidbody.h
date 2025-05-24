#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Engine/Components/Component.h"
#include "Engine/Components/Transform.h"
#include "Engine/EngineObjects/Entity.h"
namespace Engine
{
    enum class Constraints : uint32_t
    {
        None = 0,
        LockPositionX = 1 << 0,
        LockPositionY = 1 << 1,
        LockPositionZ = 1 << 2,
        LockRotationX = 1 << 3,
        LockRotationY = 1 << 4,
        LockRotationZ = 1 << 5,
        LockAllPosition = LockPositionX | LockPositionY | LockPositionZ,
        LockAllRotation = LockRotationX | LockRotationY | LockRotationZ,
        LockAll = LockAllPosition | LockAllRotation
    };

    inline Constraints operator|(Constraints a, Constraints b)
    {
        return static_cast<Constraints>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline Constraints operator&(Constraints a, Constraints b)
    {
        return static_cast<Constraints>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline Constraints& operator|=(Constraints& a, Constraints b)
    {
        a = a | b;
        return a;
    }

    inline bool HasConstraint(Constraints value, Constraints flag) { return (value & flag) != Constraints::None; }

    class RigidBody : public Component
    {
    public:
        RigidBody();
        ~RigidBody() override = default;

        void AddForce(const glm::vec3& force);
        void AddTorque(const glm::vec3& torque);
        void AddImpulse(const glm::vec3& impulse);
        void AddAngularImpulse(const glm::vec3& angularImpulse);

        void SetMass(float newMass)
        {
            mass = newMass;
            inverseMass = (mass > 0.0f) ? 1.0f / mass : 0.0f;
        }

        float GetMass() const { return mass; }

        void SetRestitution(float value) { restitution = value; }
        float GetRestitution() const { return restitution; }

        void SetLinearDamping(float damping) { linearDamping = damping; }
        float GetLinearDamping() const { return linearDamping; }

        void LockRotationX(bool lock) { lockRotationX = lock; }
        void LockRotationY(bool lock) { lockRotationY = lock; }
        void LockRotationZ(bool lock) { lockRotationZ = lock; }

        void LockPositionX(bool lock) { lockPositionX = lock; }
        void LockPositionY(bool lock) { lockPositionY = lock; }
        void LockPositionZ(bool lock) { lockPositionZ = lock; }

        void SetConstraints(Constraints c) { constraints = c; }
        Constraints GetConstraints() const { return constraints; }
        bool IsConstraintEnabled(Constraints c) const { return HasConstraint(constraints, c); }
        void AddConstraint(Constraints c) { constraints |= c; }
        void RemoveConstraint(Constraints c)
        {
            constraints = static_cast<Constraints>(static_cast<uint32_t>(constraints) & ~static_cast<uint32_t>(c));
        }

        void OnCollision(RigidBody& other, const glm::vec3& contactPoint, const glm::vec3& collisionNormal);
        void OnCollision_Static(const glm::vec3& contactPoint, const glm::vec3& collisionNormal);

        void Start() override;
        void OnDestroy() override;
        void Update(float DeltaTime);

        SERIALIZATION_EXPORT_CLASS(RigidBody)

        #if EDITOR
        void DrawImGui() override;
        #endif

        glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);

        Transform* transform;

        bool lockPositionX = false;
        bool lockPositionY = false;
        bool lockPositionZ = false;

        bool lockRotationX = false;
        bool lockRotationY = false;
        bool lockRotationZ = false;

        Constraints constraints = Constraints::None;

        float linearDamping = 0.98f;
        float angularDamping = 0.98f;
        float frictionCoefficient = 0.5f;

        float restitution = 0.5f;
        float inverseMass = 1.0f;

        float mass;
        glm::vec3 inertiaTensor;
        glm::vec3 inverseInertiaTensor;

        glm::vec3 velocity = glm::vec3(0.0f);
        glm::vec3 angularVelocity = glm::vec3(0.0f);

    private:
        
        glm::vec3 accumulatedForce = glm::vec3(0.0f);
        glm::vec3 accumulatedTorque = glm::vec3(0.0f);

        void ApplyConstraints();

    

    };
} // namespace Engine
