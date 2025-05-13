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
                                float penetrationDepth)
    {
        float velocityAlongNormal = glm::dot(linearVelocity, collisionNormal);

        if (velocityAlongNormal > 0.0f)
            return;

        float restitutionFactor = restitution;

        float impulseMagnitude = -(1.0f + restitutionFactor) * velocityAlongNormal;
        impulseMagnitude /= inverseMass;

        glm::vec3 impulse = impulseMagnitude * collisionNormal;

        linearVelocity += impulse * inverseMass;

        const float correctionFactor = 0.8f;
        glm::vec3 correction = correctionFactor * penetrationDepth * collisionNormal;
        GetOwner()->GetTransform()->SetPosition(GetOwner()->GetTransform()->GetPosition() + correction);
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
        // TODO: remove when scriptable fully implemented
        if (timeSinceLastForce >= 10.0f)
        {
            AddForce(glm::vec3(-.5f, 0.0f, 0.0f), ForceType::Impulse);
            timeSinceLastForce = 0.0f;
        }
        timeSinceLastForce += deltaTime;
        // TODO END
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
