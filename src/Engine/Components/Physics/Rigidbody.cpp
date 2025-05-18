#include "Rigidbody.h"
#include "../Colliders/Collider.h"

namespace Engine
{

    RigidBody::RigidBody() :
        mass(1.0f), inverseMass(1.0f), linearVelocity(0.0f), angularVelocity(0.0f),
        orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), accumulatedForce(0.0f), accumulatedTorque(0.0f),
        staticFriction(0.0f), dynamicFriction(0.0f), restitution(0.5f), linearDamping(0.001f), angularDamping(0.001f),
        constraints(Constraints::None)
    {
        UpdateManager::GetInstance()->RegisterComponent(this);
    }

    RigidBody::~RigidBody() 
    { 
        UpdateManager::GetInstance()->UnregisterComponent(this); 
    }

    RigidBody& RigidBody::operator=(const RigidBody& other)
    {
        if (this == &other)
            return *this;

        mass = other.mass;
        inverseMass = other.inverseMass;
        linearVelocity = other.linearVelocity;
        angularVelocity = other.angularVelocity;
        orientation = other.orientation;

        accumulatedForce = other.accumulatedForce;
        accumulatedTorque = other.accumulatedTorque;

        staticFriction = other.staticFriction;
        dynamicFriction = other.dynamicFriction;
        restitution = other.restitution;
        linearDamping = other.linearDamping;
        angularDamping = other.angularDamping;

        constraints = other.constraints;

        return *this;
    }

    void RigidBody::OnCollision(const glm::vec3& collisionNormal, const glm::vec3& collisionPoint,
                                float penetrationDepth, RigidBody* otherBody)
    {
        if (penetrationDepth < 0.05f && glm::dot(collisionNormal, glm::vec3(0, 1, 0)) > 0.7f)
        {
            // jeœli prêdkoœæ pionowa jest ma³a lub skierowana w dó³
            if (linearVelocity.y < 0.1f)
            {
                linearVelocity.y = 0.0f; // blokujemy "przenikanie" i drgania
            }
        }

        const float minPenetrationThreshold = 0.01f;
        if (penetrationDepth < minPenetrationThreshold)
            return;

        // Upewnij siê, ¿e penetrationDepth jest dodatnie
        float penetration = std::max(0.0f, penetrationDepth);

        glm::vec3 normal = glm::normalize(collisionNormal);

        if (!otherBody || otherBody->inverseMass == 0.0f)
        {
            float velocityAlongNormal = glm::dot(linearVelocity, normal);
            if (velocityAlongNormal > 0.0f)
                return;

            float impulseMagnitude = -(1.0f + restitution) * velocityAlongNormal;
            glm::vec3 impulse = impulseMagnitude * normal;
            linearVelocity += impulse;

            // Korekcja pozycji
            const float correctionFactor = 0.8f;
            glm::vec3 correction = correctionFactor * penetration * normal;
            GetOwner()->GetTransform()->SetPosition(GetOwner()->GetTransform()->GetPosition() + correction);
            return;
        }

        // Oblicz impuls z uwzglêdnieniem obu cia³
        float e = (restitution + otherBody->restitution) * 0.5f;
        glm::vec3 relativeVelocity = linearVelocity - otherBody->linearVelocity;
        float velAlongNormal = glm::dot(relativeVelocity, normal);

        if (velAlongNormal > 0.0f)
            return;

        float invMassSum = inverseMass + otherBody->inverseMass;
        float j = -(1.0f + e) * velAlongNormal / invMassSum;

        glm::vec3 impulse = j * normal;

        linearVelocity += impulse * inverseMass;
        otherBody->linearVelocity -= impulse * otherBody->inverseMass;

        const float correctionFactor = 0.8f;
        float totalInvMass = inverseMass + otherBody->inverseMass;
        glm::vec3 correction = correctionFactor * penetration * normal;

        glm::vec3 correctionThis = correction * (inverseMass / totalInvMass);
        glm::vec3 correctionOther = -correction * (otherBody->inverseMass / totalInvMass);

        GetOwner()->GetTransform()->SetPosition(GetOwner()->GetTransform()->GetPosition() + correctionThis);
        otherBody->GetOwner()->GetTransform()->SetPosition(otherBody->GetOwner()->GetTransform()->GetPosition() +
                                                           correctionOther);
    }

    void RigidBody::SetConstraints(Constraints newConstraints) { constraints = newConstraints; }

    void RigidBody::AddConstraint(Constraints constraint) { constraints |= constraint; }

    void RigidBody::RemoveConstraint(Constraints constraint) { constraints &= ~constraint; }

    bool RigidBody::HasConstraint(Constraints constraint) const
    {
        return (constraints & constraint) != Constraints::None;
    }

    void RigidBody::Start()
    {

    }

    void RigidBody::Update(float deltaTime)
    {
        
        if (inverseMass == 0.0f)
            return;

        if (gravityEnabled && inverseMass > 0.0f)
        {
            accumulatedForce += gravity * mass;
        }

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

        Transform* transform = GetOwner()->GetTransform();
        glm::vec3 position = transform->GetPosition();
        position += linearVelocity * deltaTime;
        GetOwner()->GetTransform()->SetPosition(position);

        glm::quat angularDelta = glm::quat(0.0f, angularVelocity * deltaTime);
        orientation = glm::normalize(orientation + angularDelta * orientation);
        GetOwner()->GetTransform()->SetEulerAngles(glm::eulerAngles(orientation));

        accumulatedForce = glm::vec3(0.0f);
        accumulatedTorque = glm::vec3(0.0f);
    }

    
    void RigidBody::AddForce(const glm::vec3& force, ForceType type)
    {
        if (type == ForceType::Impulse)
        {
            linearVelocity += force * inverseMass;
        }
        else
        {
            accumulatedForce += force;
        }
    }

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

    void RigidBody::EnableGravity(bool enabled) { gravityEnabled = enabled; }

    bool RigidBody::IsGravityEnabled() const { return gravityEnabled; }

    void RigidBody::SetGravity(const glm::vec3& newGravity) { gravity = newGravity; }

    const glm::vec3& RigidBody::GetGravity() const { return gravity; }

    rapidjson::Value RigidBody::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        END_COMPONENT_SERIALIZATION
    }

    void RigidBody::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
       
    }

    void RigidBody::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                Serialization::ReferenceTable& ReferenceMap)
    {
    }


} // namespace Engine
