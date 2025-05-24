#include "RigidBody.h"
#include "Serialization/SerializationUtility.h"
#include "Engine/Components/Transform.h" // Twoja klasa Transform
#include "Engine/EngineObjects/RigidbodyUpdateManager.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/Components/Colliders/Collider.h"

namespace Engine
{
    RigidBody::RigidBody()
    {
        mass = 1.0f;
        inverseMass = 1.0f / mass;
        restitution = 0.5f; // domyœlna wartoœæ restitucji
        frictionCoefficient = 0.5f; // domyœlna wartoœæ wspó³czynnika tarcia
        linearDamping = 0.99f; // domyœlne t³umienie liniowe
        angularDamping = 0.99f; // domyœlne t³umienie k¹towe
        inertiaTensorBody = glm::mat3(1.0f); // macierz bezw³adnoœci w uk³adzie cia³a
        
        lockPositionX = false;
        lockPositionY = false;
        lockPositionZ = false;
        lockRotationX = false;
        lockRotationY = false;
        lockRotationZ = false;
    }


    void RigidBody::UpdateInertiaTensorWorld()
    {
        glm::mat3 rotationMatrix = glm::mat3_cast(transform->GetRotation());
        inverseInertiaTensorMatrix = glm::inverse(rotationMatrix * inertiaTensorBody * glm::transpose(rotationMatrix));
    }

    

    void RigidBody::AddForce(const glm::vec3& force) { accumulatedForce += force; }

    void RigidBody::AddTorque(const glm::vec3& torque) { accumulatedTorque += torque; }

    void RigidBody::AddImpulse(const glm::vec3& impulse)
    {
        if (mass <= 0)
            return; // nieruchome cia³o

        velocity += impulse / mass;
    }

    void RigidBody::StabilizeIfUpright()
    {
        glm::mat4 modelMatrix = transform->GetLocalToWorldMatrix();
        glm::vec3 upLocal = glm::normalize(glm::vec3(modelMatrix[1])); // Y axis

        glm::vec3 worldUp = glm::vec3(0, 1, 0);
        float dot = glm::dot(upLocal, worldUp);
        float angle = glm::acos(glm::clamp(dot, -1.0f, 1.0f)); // radians

        if (glm::degrees(angle) < 45.0f)
        {
            // Stabilize: slerp back toward upright rotation
            glm::quat currentRot = transform->GetRotation();
            glm::quat uprightRot = glm::quatLookAt(glm::vec3(0, 0, -1), worldUp); // forward, up

            // Avoid flipping when current rotation is close enough
            glm::quat newRot = glm::slerp(currentRot, uprightRot, 4.0f); // smooth, tune factor
            transform->SetRotation(glm::normalize(newRot));
        }
    }


    void RigidBody::AddAngularImpulse(const glm::vec3& angularImpulse)
    {
        angularVelocity += inverseInertiaTensorMatrix * angularImpulse;
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
        transform = GetOwner()->GetTransform();
        RigidbodyUpdateManager::GetInstance()->RegisterRigidbody(this);
        
        velocity = glm::vec3(0.0f);
        angularVelocity = glm::vec3(0.0f);
        accumulatedForce = glm::vec3(0.0f);
        accumulatedTorque = glm::vec3(0.0f);
        mass = std::max(mass, 0.001f); // zabezpieczenie

        Collider* collider = GetOwner()->GetComponent<Collider>();
        if (collider != nullptr)
        {
            inertiaTensorBody = collider->CalculateInertiaTensorBody(mass);
        }

        UpdateInertiaTensorWorld();
        inverseInertiaTensorMatrix = glm::inverse(inertiaTensorBody);
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
        glm::vec3 angularAcceleration = inverseInertiaTensorMatrix * accumulatedTorque;

        velocity += acceleration * deltaTime;
        angularVelocity += angularAcceleration * deltaTime;

        velocity *= linearDamping;
        angularVelocity *= angularDamping;

        glm::vec3 position = transform->GetPositionWorldSpace();
        position += velocity * deltaTime;
        

        glm::quat rotation = transform->GetRotation();

        glm::quat angularVelQuat(0.0f, angularVelocity.x, angularVelocity.y, angularVelocity.z);
        glm::quat deltaRotation = 0.5f * angularVelQuat * rotation;

        rotation += deltaRotation * deltaTime;
        rotation = glm::normalize(rotation);

       

        accumulatedForce = glm::vec3(0.0f);
        accumulatedTorque = glm::vec3(0.0f);

        ApplyConstraints();
        
        transform->SetPositionWorldSpace(position);
        transform->SetRotation(rotation);

        UpdateInertiaTensorWorld();
        //ApplyUprightStabilization(); // Stabilizacja pionowa, jeœli potrzebna
    }

    void RigidBody::OnCollision_Static(const glm::vec3& contactPoint, const glm::vec3& collisionNormal)
    {
        float velocityAlongNormal = glm::dot(velocity, collisionNormal);

        if (velocityAlongNormal > 0.0f)
            return;

        float invMass = (mass > 0.0f) ? 1.0f / mass : 0.0f;
        if (invMass == 0.0f)
            return;

        // Mniejsza restitucja na statycznym pod³o¿u
        float localRestitution = 0.05f;

        float j = -(1.0f + localRestitution) * velocityAlongNormal;

        // Limitowanie impulsu
        const float maxImpulse = 5.0f;
        if (j > maxImpulse)
            j = maxImpulse;

        glm::vec3 impulse = j * collisionNormal;
        velocity += impulse * invMass;

        // Dodatkowe t³umienie prêdkoœci liniowej
        velocity *= 0.5f;

        // T³umienie prêdkoœci k¹towej
        angularVelocity *= 0.5f;

        // Tarcie liniowe
        glm::vec3 tangent = velocity - glm::dot(velocity, collisionNormal) * collisionNormal;
        if (glm::length(tangent) > 0.0001f)
        {
            tangent = glm::normalize(tangent);
            glm::vec3 frictionImpulse = -frictionCoefficient * tangent;
            velocity += frictionImpulse * invMass;
        }

        // Korekcja pozycji mniejsz¹ wartoœci¹
        const float penetrationDepth = 0.001f;
        glm::vec3 pos = transform->GetPositionWorldSpace();
        pos += collisionNormal * penetrationDepth;
        transform->SetPositionWorldSpace(pos);
    }


   void RigidBody::ResolvePenetration(RigidBody& other, const glm::vec3& collisionNormal, float penetrationDepth)
    {
        float invMassA = (mass > 0) ? 1.0f / mass : 0.0f;
        float invMassB = (other.mass > 0) ? 1.0f / other.mass : 0.0f;
        float totalInvMass = invMassA + invMassB;

        if (totalInvMass == 0.0f || penetrationDepth <= 0.0f)
            return;

        // Penetration correction constants
        const float k_slop = 0.01f;
        const float k_percent = 0.8f;

        float correctionMagnitude = std::max(penetrationDepth - k_slop, 0.0f) / totalInvMass * k_percent;
        glm::vec3 correction = correctionMagnitude * collisionNormal;

        glm::vec3 posA = transform->GetPositionWorldSpace();
        glm::vec3 posB = other.GetOwner()->GetTransform()->GetPositionWorldSpace();

        posA -= correction * invMassA;
        posB += correction * invMassB;

        transform->SetPositionWorldSpace(posA);
        other.GetOwner()->GetTransform()->SetPositionWorldSpace(posB);

        // Optional: stabilize orientation if nearly upright
        StabilizeIfUpright();
    }


   void RigidBody::ApplyUprightStabilization()
    {
        // Próg k¹ta — jeœli odchylenie jest mniejsze ni¿ 45°, stabilizujemy
        const float angleThreshold = glm::radians(45.0f);
        const float torqueStrength = 5.0f; // Im wiêksza wartoœæ, tym szybciej siê stabilizuje

        glm::vec3 upWorld = glm::vec3(0, 1, 0);
        glm::vec3 bodyUp = transform->GetUp(); // np. glm::vec3(transform->GetMatrix()[1])

        float angle = glm::acos(glm::clamp(glm::dot(upWorld, bodyUp), -1.0f, 1.0f));

        if (angle < angleThreshold)
        {
            glm::vec3 axis = glm::cross(bodyUp, upWorld);
            if (glm::dot(axis, axis) > 1e-6f)
            {
                axis = glm::normalize(axis);
                glm::vec3 correctiveTorque = axis * angle * torqueStrength;
                AddTorque(correctiveTorque); // Musisz mieæ wsparcie momentu obrotowego
            }
        }
    }


    void RigidBody::ResolveCollisionVelocity(RigidBody& other, const glm::vec3& contactPoint,
                                             const glm::vec3& collisionNormal)
    {
        Transform* otherTransform = other.GetOwner()->GetTransform();

        glm::vec3 rA = contactPoint - transform->GetPositionWorldSpace();
        glm::vec3 rB = contactPoint - otherTransform->GetPositionWorldSpace();

        glm::vec3 vRel =
                velocity + glm::cross(angularVelocity, rA) - other.velocity - glm::cross(other.angularVelocity, rB);

        float e = std::min(restitution, other.restitution); // wsp. odbicia (mo¿na dobieraæ)

        float invMassA = (mass > 0) ? 1.0f / mass : 0.0f;
        float invMassB = (other.mass > 0) ? 1.0f / other.mass : 0.0f;

        glm::mat3 rotationA = glm::mat3_cast(transform->GetRotation());
        glm::mat3 rotationB = glm::mat3_cast(otherTransform->GetRotation());

        // macierze inercji w przestrzeni œwiata
        glm::mat3 invInertiaA = rotationA * inverseInertiaTensorMatrix * glm::transpose(rotationA);
        glm::mat3 invInertiaB = rotationB * other.inverseInertiaTensorMatrix * glm::transpose(rotationB);

        // momenty obrotowe
        glm::vec3 raCrossN = glm::cross(rA, collisionNormal);
        glm::vec3 rbCrossN = glm::cross(rB, collisionNormal);

        float angularEffectA = glm::dot(raCrossN, invInertiaA * raCrossN);
        float angularEffectB = glm::dot(rbCrossN, invInertiaB * rbCrossN);

        float denominator = invMassA + invMassB + angularEffectA + angularEffectB;
        if (denominator == 0.0f)
            return;

        float j = -(1.0f + e) * glm::dot(vRel, collisionNormal) / denominator;

        glm::vec3 impulse = j * collisionNormal;

        AddImpulse(impulse);
        AddAngularImpulse(glm::cross(rA, impulse));

        other.AddImpulse(-impulse);
        other.AddAngularImpulse(glm::cross(rB, -impulse));

        // Korekcja tarcia
        glm::vec3 tangent = vRel - glm::dot(vRel, collisionNormal) * collisionNormal;
        if (glm::length(tangent) > 0.0001f)
        {
            tangent = glm::normalize(tangent);
            float frictionImpulseMag = -frictionCoefficient * j;
            glm::vec3 frictionImpulse = tangent * frictionImpulseMag;

            AddImpulse(frictionImpulse);
            other.AddImpulse(-frictionImpulse);
        }
    }

    void RigidBody::OnCollision(RigidBody& other, const glm::vec3& contactPoint, const glm::vec3& collisionNormal,
                                float penetrationDepth)
    {
        // 1. Korekcja pozycji (penetracji)
        ResolvePenetration(other, collisionNormal, penetrationDepth);

        // 2. Korekcja prêdkoœci (impulsy)
        ResolveCollisionVelocity(other, contactPoint, collisionNormal);
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
