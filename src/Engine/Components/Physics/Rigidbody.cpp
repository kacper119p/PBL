#include "RigidBody.h"
#include "Serialization/SerializationUtility.h"
#include "Engine/Components/Transform.h" // Twoja klasa Transform
#include "Engine/EngineObjects/RigidbodyUpdateManager.h"
#include "Engine/EngineObjects/Entity.h"

namespace Engine
{
    RigidBody::RigidBody()
    {
        transform = GetOwner()->GetTransform();
        mass = .1f; // Domyœlna masa
        inertiaTensor = glm::vec3(1.0f, 1.0f, 1.0f); // Domyœlny tensor bezw³adnoœci
        inverseInertiaTensor = glm::vec3(1.0f, 1.0f, 1.0f); // Domyœlny odwrotny tensor bezw³adnoœci
        
    }

    void RigidBody::AddForce(const glm::vec3& force) { accumulatedForce += force; }

    void RigidBody::AddTorque(const glm::vec3& torque) { accumulatedTorque += torque; }

    void RigidBody::AddImpulse(const glm::vec3& impulse)
    {
        if (mass <= 0)
            return; // nieruchome cia³o

        velocity += impulse / mass;
    }

    void RigidBody::AddAngularImpulse(const glm::vec3& angularImpulse)
    {
        angularVelocity +=
                glm::vec3(angularImpulse.x * inverseInertiaTensor.x, angularImpulse.y * inverseInertiaTensor.y,
                          angularImpulse.z * inverseInertiaTensor.z);
    }

    void RigidBody::ApplyConstraints()
    {

        glm::vec3 pos = transform->GetPositionWorldSpace();
        if (lockPositionX)
            pos.x = 0.0f;
        if (lockPositionY)
            pos.y = 0.0f;
        if (lockPositionZ)
            pos.z = 0.0f;
        transform->SetPositionWorldSpace(pos);

        if (lockRotationX)
            angularVelocity.x = 0.0f;
        if (lockRotationY)
            angularVelocity.y = 0.0f;
        if (lockRotationZ)
            angularVelocity.z = 0.0f;
    }

    void RigidBody::Start() {
        RigidbodyUpdateManager::GetInstance()->RegisterRigidbody(this);
        velocity = glm::vec3(0.0f);
        angularVelocity = glm::vec3(0.0f);
        accumulatedForce = glm::vec3(0.0f);
        accumulatedTorque = glm::vec3(0.0f);
        ApplyConstraints();
    }

    void RigidBody::OnDestroy() {
        RigidbodyUpdateManager::GetInstance()->UnregisterRigidbody(this);
    }

    void RigidBody::Update(float deltaTime)
    {
        if (mass <= 0)
            return;

        // Dodaj grawitacjê (tylko jeœli cia³o jest ruchome)
        accumulatedForce += gravity * mass;

        glm::vec3 acceleration = accumulatedForce / mass;
        glm::vec3 angularAcceleration =
                glm::vec3(accumulatedTorque.x * inverseInertiaTensor.x, accumulatedTorque.y * inverseInertiaTensor.y,
                          accumulatedTorque.z * inverseInertiaTensor.z);

        velocity += acceleration * deltaTime;
        angularVelocity += angularAcceleration * deltaTime;

        velocity *= linearDamping;
        angularVelocity *= angularDamping;

        glm::vec3 position = transform->GetPositionWorldSpace();
        position += velocity * deltaTime;
        transform->SetPositionWorldSpace(position);

        glm::quat rotation = transform->GetRotation();

        glm::quat angularVelQuat(0.0f, angularVelocity.x, angularVelocity.y, angularVelocity.z);
        glm::quat deltaRotation = 0.5f * angularVelQuat * rotation;

        rotation += deltaRotation * deltaTime;
        rotation = glm::normalize(rotation);

        transform->SetRotation(rotation);

        accumulatedForce = glm::vec3(0.0f);
        accumulatedTorque = glm::vec3(0.0f);

        ApplyConstraints();
    }


    void RigidBody::OnCollision_Static(const glm::vec3& contactPoint, const glm::vec3& collisionNormal)
    {

        float velocityAlongNormal = glm::dot(velocity, collisionNormal);

        if (velocityAlongNormal > 0.0f)
            return;

        float j = -(1.0f + restitution) * velocityAlongNormal;
        glm::vec3 impulse = j * collisionNormal;

        velocity += impulse * inverseMass;

        glm::vec3 tangent = glm::normalize(velocity - glm::dot(velocity, collisionNormal) * collisionNormal);
        glm::vec3 frictionImpulse = -frictionCoefficient * tangent;
        velocity += frictionImpulse * inverseMass;

        const float penetrationDepth = 0.01f;
        glm::vec3 correction = collisionNormal * penetrationDepth;
        glm::vec3 pos = transform->GetPositionWorldSpace();
        transform->SetPosition(pos + correction);
    }


    void RigidBody::OnCollision(RigidBody& other, const glm::vec3& contactPoint, const glm::vec3& collisionNormal)
    {
        Transform* otherTransform = other.GetOwner()->GetTransform();

        glm::vec3 rA = contactPoint - transform->GetPositionWorldSpace();
        glm::vec3 rB = contactPoint - otherTransform->GetPositionWorldSpace();

        glm::vec3 vRel =
                velocity + glm::cross(angularVelocity, rA) - other.velocity - glm::cross(other.angularVelocity, rB);

        float e = 0.8f;

        float invMassSum = (mass > 0 ? 1.0f / mass : 0.0f) + (other.mass > 0 ? 1.0f / other.mass : 0.0f);

        float angularEffectA =
                glm::dot(glm::cross(rA, collisionNormal), glm::cross(rA, collisionNormal)) * inverseInertiaTensor.x;
        float angularEffectB = glm::dot(glm::cross(rB, collisionNormal), glm::cross(rB, collisionNormal)) *
                               other.inverseInertiaTensor.x;

        float j = -(1.0f + e) * glm::dot(vRel, collisionNormal) / (invMassSum + angularEffectA + angularEffectB);

        glm::vec3 impulse = collisionNormal * j;

        AddImpulse(impulse);
        AddAngularImpulse(glm::cross(rA, impulse));

        const float penetrationDepth = 0.01f;
        glm::vec3 posA = transform->GetPositionWorldSpace();
        glm::vec3 posB = otherTransform->GetPositionWorldSpace();

        float invMassA = (mass > 0) ? 1.0f / mass : 0.0f;
        float invMassB = (other.mass > 0) ? 1.0f / other.mass : 0.0f;
        float totalInvMass = invMassA + invMassB;

        if (totalInvMass > 0.0f)
        {
            posA -= collisionNormal * penetrationDepth * (invMassA / totalInvMass);
            posB += collisionNormal * penetrationDepth * (invMassB / totalInvMass);

            transform->SetPositionWorldSpace(posA);        }

        glm::vec3 tangent = vRel - collisionNormal * glm::dot(vRel, collisionNormal);
        if (glm::length(tangent) > 0.0001f)
        {
            tangent = glm::normalize(tangent);
            float frictionImpulseMag = -frictionCoefficient * j;
            glm::vec3 frictionImpulse = tangent * frictionImpulseMag;

            AddImpulse(frictionImpulse);
            other.AddImpulse(-frictionImpulse);
        }
    }

    #if EDITOR
    void RigidBody::DrawImGui()
    {
        if (ImGui::CollapsingHeader("RigidBody"))
        {
            ImGui::DragFloat("Mass", &mass, 0.1f, 0.0f, FLT_MAX);
            inverseMass = (mass > 0.0f) ? 1.0f / mass : 0.0f;

            ImGui::DragFloat("Restitution", &restitution, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Friction", &frictionCoefficient, 0.01f, 0.0f, 10.0f);

            ImGui::DragFloat("Linear Damping", &linearDamping, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Angular Damping", &angularDamping, 0.01f, 0.0f, 1.0f);

            if (ImGui::TreeNode("Constraints"))
            {
                bool posX = IsConstraintEnabled(Constraints::LockPositionX);
                bool posY = IsConstraintEnabled(Constraints::LockPositionY);
                bool posZ = IsConstraintEnabled(Constraints::LockPositionZ);

                bool rotX = IsConstraintEnabled(Constraints::LockRotationX);
                bool rotY = IsConstraintEnabled(Constraints::LockRotationY);
                bool rotZ = IsConstraintEnabled(Constraints::LockRotationZ);

                if (ImGui::Checkbox("Lock Position X", &posX))
                    posX ? AddConstraint(Constraints::LockPositionX) : RemoveConstraint(Constraints::LockPositionX);
                if (ImGui::Checkbox("Lock Position Y", &posY))
                    posY ? AddConstraint(Constraints::LockPositionY) : RemoveConstraint(Constraints::LockPositionY);
                if (ImGui::Checkbox("Lock Position Z", &posZ))
                    posZ ? AddConstraint(Constraints::LockPositionZ) : RemoveConstraint(Constraints::LockPositionZ);

                if (ImGui::Checkbox("Lock Rotation X", &rotX))
                    rotX ? AddConstraint(Constraints::LockRotationX) : RemoveConstraint(Constraints::LockRotationX);
                if (ImGui::Checkbox("Lock Rotation Y", &rotY))
                    rotY ? AddConstraint(Constraints::LockRotationY) : RemoveConstraint(Constraints::LockRotationY);
                if (ImGui::Checkbox("Lock Rotation Z", &rotZ))
                    rotZ ? AddConstraint(Constraints::LockRotationZ) : RemoveConstraint(Constraints::LockRotationZ);

                ImGui::TreePop();
            }
        }
    }
    #endif

     rapidjson::Value RigidBody::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {

        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(mass);
        SERIALIZE_FIELD(inertiaTensor);
        SERIALIZE_FIELD(inverseInertiaTensor);
        SERIALIZE_FIELD(linearDamping);
        SERIALIZE_FIELD(angularDamping);
        SERIALIZE_FIELD(frictionCoefficient);
        SERIALIZE_FIELD(restitution);
        SERIALIZE_FIELD(inverseMass);
        SERIALIZE_FIELD(lockPositionX);
        SERIALIZE_FIELD(lockPositionY);
        SERIALIZE_FIELD(lockPositionZ);
        SERIALIZE_FIELD(lockRotationX);
        SERIALIZE_FIELD(lockRotationY);
        SERIALIZE_FIELD(lockRotationZ);
        END_COMPONENT_SERIALIZATION
    }

    void RigidBody::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(mass);
        DESERIALIZE_VALUE(inertiaTensor);
        DESERIALIZE_VALUE(inverseInertiaTensor);
        DESERIALIZE_VALUE(linearDamping);
        DESERIALIZE_VALUE(angularDamping);
        DESERIALIZE_VALUE(frictionCoefficient);
        DESERIALIZE_VALUE(restitution);
        DESERIALIZE_VALUE(inverseMass);
        DESERIALIZE_VALUE(lockPositionX);
        DESERIALIZE_VALUE(lockPositionY);
        DESERIALIZE_VALUE(lockPositionZ);
        DESERIALIZE_VALUE(lockRotationX);
        DESERIALIZE_VALUE(lockRotationY);
        DESERIALIZE_VALUE(lockRotationZ);
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void RigidBody::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                Serialization::ReferenceTable& ReferenceMap)
    {

        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        DESERIALIZE_POINTER(transform)
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }

} // namespace Engine
