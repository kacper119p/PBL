#include "Rigidbody.h"
#include "../Colliders/Collider.h"

namespace Engine
{

    RigidBody::RigidBody() :
        mass(1.0f), inverseMass(1.0f), linearVelocity(0.0f), angularVelocity(0.0f),
        orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), accumulatedForce(0.0f), accumulatedTorque(0.0f),
        staticFriction(0.0f), dynamicFriction(0.0f), restitution(0.5f), linearDamping(0.001f), angularDamping(0.001f),
        constraints(Constraints::None), inertiaTensor(1.0f), inverseInertiaTensor(1.0f), gravityEnabled(true),
        gravity(glm::vec3(0.0f, -9.81f, 0.0f))
    {
        UpdateManager::GetInstance()->RegisterComponent(this);
    }

    RigidBody::~RigidBody() { UpdateManager::GetInstance()->UnregisterComponent(this); }

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

        inertiaTensor = other.inertiaTensor;
        inverseInertiaTensor = other.inverseInertiaTensor;

        gravityEnabled = other.gravityEnabled;
        gravity = other.gravity;

        return *this;
    }

    void RigidBody::UpdateInertiaTensor()
    {
        auto collider = GetOwner()->GetComponent<Collider>();
        if (collider)
        {
            inertiaTensor = collider->CalculateInertiaTensor(mass);
            inverseInertiaTensor = glm::inverse(inertiaTensor);
        }
        else
        {
            inertiaTensor = glm::mat3(1.0f);
            inverseInertiaTensor = glm::mat3(1.0f);
        }
    }

    void RigidBody::SetMass(float m)
    {
        mass = m;
        inverseMass = (m == 0.0f) ? 0.0f : 1.0f / m;
        UpdateInertiaTensor();
    }

    void RigidBody::SetInverseMass(float invM)
    {
        inverseMass = invM;
        mass = (invM == 0.0f) ? 0.0f : 1.0f / invM;
        UpdateInertiaTensor();
    }

    void RigidBody::OnCollision(const glm::vec3& collisionNormal, const glm::vec3& collisionPoint,
                                float penetrationDepth, RigidBody* otherBody)
    {
        if (penetrationDepth < 0.05f && glm::dot(collisionNormal, glm::vec3(0, 1, 0)) > 0.7f)
        {
            // je�li pr�dko�� pionowa jest ma�a lub skierowana w d�
            if (linearVelocity.y < 0.1f)
            {
                linearVelocity.y = 0.0f; // blokujemy "przenikanie" i drgania
            }
        }

        const float minPenetrationThreshold = 0.01f;
        if (penetrationDepth < minPenetrationThreshold)
            return;

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

            const float correctionFactor = 0.8f;
            glm::vec3 correction = correctionFactor * penetration * normal;
            GetOwner()->GetTransform()->SetPosition(GetOwner()->GetTransform()->GetPosition() + correction);
            return;
        }

        // Oblicz impuls z uwzgl�dnieniem obu cia�
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
        UpdateInertiaTensor();
    }

    void RigidBody::Update(float deltaTime)
    {
        if (inverseMass == 0.0f)
            return;

        if (gravityEnabled)
        {
            accumulatedForce += gravity * mass;
        }

        // Aktualizacja pr�dko�ci liniowej z uwzgl�dnieniem constraint�w
        if (!HasConstraint(Constraints::LockPositionX))
            linearVelocity.x += (accumulatedForce.x * inverseMass) * deltaTime;
        if (!HasConstraint(Constraints::LockPositionY))
            linearVelocity.y += (accumulatedForce.y * inverseMass) * deltaTime;
        if (!HasConstraint(Constraints::LockPositionZ))
            linearVelocity.z += (accumulatedForce.z * inverseMass) * deltaTime;

        linearVelocity *= (1.0f - linearDamping);

        // Obliczenie przyspieszenia k�towego (w przestrzeni �wiata)
        glm::mat3 R = glm::mat3_cast(orientation);
        glm::mat3 worldInvInertiaTensor = R * inverseInertiaTensor * glm::transpose(R);
        glm::vec3 angularAcceleration = worldInvInertiaTensor * accumulatedTorque;

        // Aktualizacja pr�dko�ci k�towej z uwzgl�dnieniem constraint�w
        if (!HasConstraint(Constraints::LockRotationX))
            angularVelocity.x += angularAcceleration.x * deltaTime;
        if (!HasConstraint(Constraints::LockRotationY))
            angularVelocity.y += angularAcceleration.y * deltaTime;
        if (!HasConstraint(Constraints::LockRotationZ))
            angularVelocity.z += angularAcceleration.z * deltaTime;

        angularVelocity *= (1.0f - angularDamping);

        // Aktualizacja pozycji
        Transform* transform = GetOwner()->GetTransform();
        glm::vec3 position = transform->GetPositionWorldSpace();
        position += linearVelocity * deltaTime;
        transform->SetPosition(position);

        // Ogranicz pr�dko�� k�tow� wed�ug constraint�w przed przeliczeniem rotacji
        glm::vec3 effectiveAngularVelocity = angularVelocity;

        if (HasConstraint(Constraints::LockRotationX))
            effectiveAngularVelocity.x = 0.0f;
        if (HasConstraint(Constraints::LockRotationY))
            effectiveAngularVelocity.y = 0.0f;
        if (HasConstraint(Constraints::LockRotationZ))
            effectiveAngularVelocity.z = 0.0f;

        if (glm::length(effectiveAngularVelocity) > 0.0001f)
        {
            float angle = glm::length(effectiveAngularVelocity) * deltaTime;
            glm::vec3 axis = glm::normalize(effectiveAngularVelocity);

            if (angle > 0.00001f)
            {
                glm::quat deltaRotation = glm::angleAxis(angle, axis);
                orientation = glm::normalize(deltaRotation * orientation);
            }
        }

        // Zaktualizuj rotacj� obiektu
        transform->SetEulerAngles(glm::eulerAngles(orientation));

        // Reset si� i moment�w
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

    void RigidBody::AddTorque(const glm::vec3& torque, ForceType type)
    {
        // Przekszta�� tensor bezw�adno�ci do przestrzeni �wiata
        glm::mat3 rotationMatrix = glm::mat3_cast(orientation);
        glm::mat3 worldInvInertiaTensor = rotationMatrix * inverseInertiaTensor * glm::transpose(rotationMatrix);

        if (type == ForceType::Impulse)
        {
            angularVelocity += worldInvInertiaTensor * torque;
        }
        else
        {
            accumulatedTorque += torque;
        }
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

    void RigidBody::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap) {}

    void RigidBody::DeserializeReferencesPass(const rapidjson::Value& Object,
                                              Serialization::ReferenceTable& ReferenceMap)
    {
    }

} // namespace Engine
