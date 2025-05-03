#include "Rigidbody.h"

namespace Engine
{

    RigidBody::RigidBody() :
        mass(1.0f), inverseMass(1.0f), linearVelocity(0.0f), angularVelocity(0.0f),
        orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), accumulatedForce(0.0f), accumulatedTorque(0.0f),
        staticFriction(0.5f), dynamicFriction(0.3f), restitution(0.5f), linearDamping(0.01f), angularDamping(0.01f),
        constraints(Constraints::None)
    {
    }

    void RigidBody::SetConstraints(Constraints newConstraints) { constraints = newConstraints; }

    void RigidBody::AddConstraint(Constraints constraint) { constraints |= constraint; }

    void RigidBody::RemoveConstraint(Constraints constraint) { constraints &= ~constraint; }

    bool RigidBody::HasConstraint(Constraints constraint) const
    {
        return (constraints & constraint) != Constraints::None;
    }

    void RigidBody::Update(float deltaTime)
    {
        if (inverseMass == 0.0f)
            return;

        if (!HasConstraint(Constraints::LockPositionX))
            linearVelocity.x += (accumulatedForce.x * inverseMass) * deltaTime;
        if (!HasConstraint(Constraints::LockPositionY))
            linearVelocity.y += (accumulatedForce.y * inverseMass) * deltaTime;
        if (!HasConstraint(Constraints::LockPositionZ))
            linearVelocity.z += (accumulatedForce.z * inverseMass) * deltaTime;

        linearVelocity *= (1.0f - linearDamping);

        if (!HasConstraint(Constraints::LockRotationX))
            angularVelocity.x += accumulatedTorque.x * inverseMass * deltaTime;
        if (!HasConstraint(Constraints::LockRotationY))
            angularVelocity.y += accumulatedTorque.y * inverseMass * deltaTime;
        if (!HasConstraint(Constraints::LockRotationZ))
            angularVelocity.z += accumulatedTorque.z * inverseMass * deltaTime;

        angularVelocity *= (1.0f - angularDamping);

        auto transform = GetOwner()->GetTransform();
        glm::vec3 position = transform->GetPosition();
        position += linearVelocity * deltaTime;
        transform->SetPosition(position);

        glm::quat angularDelta = glm::quat(0.0f, angularVelocity * deltaTime);
        orientation = glm::normalize(orientation + angularDelta * orientation);
        transform->SetEulerAngles(glm::eulerAngles(orientation));

        accumulatedForce = glm::vec3(0.0f);
        accumulatedTorque = glm::vec3(0.0f);
    }

    
    void RigidBody::AddForce(const glm::vec3& force) { accumulatedForce += force; }

    void RigidBody::AddTorque(const glm::vec3& torque) { accumulatedTorque += torque; }

    void RigidBody::SetMass(float m)
    {
        mass = m;
        inverseMass = (m == 0.0f) ? 0.0f : 1.0f / m;
    }

    void RigidBody::SetInverseMass(float invM)
    {
        inverseMass = invM;
        mass = (invM == 0.0f) ? 0.0f : 1.0f / invM; 
    }

    void RigidBody::SetFriction(float muStatic, float muDynamic)
    {
        staticFriction = muStatic;
        dynamicFriction = muDynamic;
    }

    void RigidBody::SetRestitution(float e) { restitution = e; }

    void RigidBody::SetLinearDamping(float d) { linearDamping = d; }

    void RigidBody::SetAngularDamping(float d) { angularDamping = d; }

    const glm::vec3& RigidBody::GetLinearVelocity() const { return linearVelocity; }

    const glm::vec3& RigidBody::GetAngularVelocity() const { return angularVelocity; }

    const glm::quat& RigidBody::GetOrientation() const { return orientation; }

} // namespace Engine
