#include "Rigidbody.h"
#include <algorithm>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <limits>
#include "Engine/Components/Colliders/Collider.h"
#include "Engine/Components/Colliders/PrimitiveMeshes.h"
#include "Engine/Components/Renderers/ModelRenderer.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/EngineObjects/RigidbodyUpdateManager.h"
#include "Serialization/SerializationUtility.h"

namespace Engine
{

    Rigidbody::Rigidbody() :
        transform(nullptr), mass(1.0f), inverseMass(1.0f), inertiaTensor(1.0f), inverseInertiaTensor(1.0f),
        velocity(0.0f), angularVelocity(0.0f), linearDamping(0.01f), angularDamping(0.01f), friction(0.5f),
        frictionEnabled(true), restitution(0.3f), accumulatedForce(0.0f), accumulatedTorque(0.0f), lastPosition(0.0f),
        lastRotation(1, 0, 0, 0), lastCollisionNormal(0.0f, 1.0f, 0.0f), collisionNormalTimeout(0.0f),
        collisionNormalTimer(0.0f)
    {
    }

    void Rigidbody::SetMass(float m)
    {
        mass = m;
        inverseMass = (mass > 0.0f) ? 1.0f / mass : 0.0f;
        computeInertiaTensor();
    }

    void Rigidbody::computeInertiaTensor()
    {
        inertiaTensor = GetOwner()->GetComponent<Collider>()->CalculateInertiaTensorBody(mass);
        inverseInertiaTensor = glm::inverse(inertiaTensor);
    }

    void Rigidbody::AddForce(const glm::vec3& force, ForceMode mode)
    {
        if (mode == ForceMode::Force)
        {
            accumulatedForce += force;
        }
        else if (mode == ForceMode::Impulse)
        {
            velocity += force * inverseMass;
        }
    }

    void Rigidbody::AddTorque(const glm::vec3& torque, ForceMode mode)
    {
        if (mode == ForceMode::Force)
        {
            accumulatedTorque += torque;
        }
        else if (mode == ForceMode::Impulse)
        {
            angularVelocity += inverseInertiaTensor * torque;
        }
    }

    void Rigidbody::ApplyGravity(const glm::vec3& gravity)
    {
        if (inverseMass > 0.0f)
        {
            AddForce(gravity * mass);
        }
    }

    void Rigidbody::Update(float deltaTime)
    {
        if (!transform)
            return;

        ApplyGravity(glm::vec3(0.0f, -9.81f, 0.0f));
        ComputeGravityTorqueFromVertices();
        lastPosition = transform->GetPosition();
        lastRotation = transform->GetRotation();

        glm::vec3 acceleration = accumulatedForce * inverseMass;
        velocity += acceleration * deltaTime;
        velocity *= (1.0f - linearDamping);

        glm::vec3 angularAcceleration = inverseInertiaTensor * accumulatedTorque;
        angularVelocity += angularAcceleration * deltaTime;
        angularVelocity *= (1.0f - angularDamping);

        glm::vec3 newPosition = transform->GetPosition() + velocity * deltaTime;
        if (constraints.freezePositionX)
            newPosition.x = transform->GetPosition().x;
        if (constraints.freezePositionY)
            newPosition.y = transform->GetPosition().y;
        if (constraints.freezePositionZ)
            newPosition.z = transform->GetPosition().z;

        glm::vec3 newAngularVelocity = angularVelocity;
        if (constraints.freezeRotationX)
            newAngularVelocity.x = 0.0f;
        if (constraints.freezeRotationY)
            newAngularVelocity.y = 0.0f;
        if (constraints.freezeRotationZ)
            newAngularVelocity.z = 0.0f;

        glm::quat rotation = transform->GetRotation();
        glm::quat deltaRot = glm::quat(0, newAngularVelocity.x, newAngularVelocity.y, newAngularVelocity.z) * rotation;
        deltaRot *= 0.5f * deltaTime;
        glm::quat updatedRot = rotation + deltaRot;
        transform->SetRotation(glm::normalize(updatedRot));

        transform->SetPosition(newPosition);
        angularVelocity = newAngularVelocity;

        if (frictionEnabled)
        {
            glm::vec3 frictionForce = -velocity * friction * mass;
            AddForce(frictionForce, ForceMode::Force);
        }

        accumulatedForce = glm::vec3(0.0f);
        accumulatedTorque = glm::vec3(0.0f);
    }


    void Rigidbody::OnCollision(Rigidbody* other, const glm::vec3& contactPoint, const glm::vec3& contactNormal)
    {
        if (!transform || !other || !other->transform)
            return;

        glm::vec3 rA = contactPoint - transform->GetPosition();
        glm::vec3 rB = contactPoint - other->transform->GetPosition();

        glm::vec3 vA = velocity + glm::cross(angularVelocity, rA);
        glm::vec3 vB = other->velocity + glm::cross(other->angularVelocity, rB);
        glm::vec3 relativeVelocity = vA - vB;

        float relVelAlongNormal = glm::dot(relativeVelocity, contactNormal);
        if (relVelAlongNormal > 0.0f)
            return;

        float e = std::min(restitution, other->restitution);

        float invMassSum = inverseMass + other->inverseMass;

        glm::vec3 rA_cross_n = glm::cross(rA, contactNormal);
        glm::vec3 rB_cross_n = glm::cross(rB, contactNormal);

        glm::vec3 Iinv_rA_cross_n = inverseInertiaTensor * rA_cross_n;
        glm::vec3 Iinv_rB_cross_n = other->inverseInertiaTensor * rB_cross_n;

        float angularTerm = glm::dot(rA_cross_n, Iinv_rA_cross_n) + glm::dot(rB_cross_n, Iinv_rB_cross_n);

        float j = -(1.0f + e) * relVelAlongNormal / (invMassSum + angularTerm);

        glm::vec3 impulse = contactNormal * j;

        velocity += impulse * inverseMass;
        other->velocity -= impulse * other->inverseMass;

        angularVelocity += inverseInertiaTensor * glm::cross(rA, impulse);
        other->angularVelocity -= other->inverseInertiaTensor * glm::cross(rB, impulse);
    }

    void Rigidbody::OnCollisionStatic(const glm::vec3& contactPoint, const glm::vec3& contactNormal)
    {
        if (!transform)
            return;

        glm::vec3 normal = glm::normalize(contactNormal);
        float relativeNormalVelocity = glm::dot(velocity, normal);

        if (relativeNormalVelocity < 0.0f)
        {
            velocity -= (1.0f + restitution) * relativeNormalVelocity * normal;

            if (glm::length(velocity) < 0.01f)
            {
                velocity = glm::vec3(0.0f);
            }

            glm::vec3 correction = (0.01f - glm::dot(transform->GetPosition() - contactPoint, normal)) * normal;
            transform->SetPosition(transform->GetPosition() + correction);
        }

        lastCollisionNormal = normal;
        collisionNormalTimeout = 1.0f;
    }

    void Rigidbody::Interpolate(float alpha)
    {
        if (!transform)
            return;
        transform->SetPosition(glm::mix(lastPosition, transform->GetPosition(), alpha));
        transform->SetRotation(glm::slerp(lastRotation, transform->GetRotation(), alpha));
    }

    void Rigidbody::Start()
    {
        transform = GetOwner()->GetTransform();
        mesh = GetOwner()->GetComponent<Collider>()->GetMesh();
        RigidbodyUpdateManager::GetInstance()->RegisterRigidbody(this);
    }

    void Rigidbody::OnDestroy() { RigidbodyUpdateManager::GetInstance()->UnregisterRigidbodyImmediate(this); }


    void Rigidbody::ComputeGravityTorqueFromVertices()
    {
        glm::mat4 modelMatrix = transform->GetLocalToWorldMatrix();

        glm::vec3 localX = glm::normalize(glm::vec3(modelMatrix[0]));
        glm::vec3 localY = glm::normalize(glm::vec3(modelMatrix[1]));
        glm::vec3 localZ = glm::normalize(glm::vec3(modelMatrix[2]));

        glm::vec3 gravityDir = glm::normalize(gravity);

        struct AxisInfo
        {
            glm::vec3 axis;
            float angle;
        };
        AxisInfo bestAxis = {localX, glm::degrees(glm::acos(glm::abs(glm::dot(localX, gravityDir))))};

        float angleY = glm::degrees(glm::acos(glm::abs(glm::dot(localY, gravityDir))));
        if (angleY < bestAxis.angle)
            bestAxis = {localY, angleY};

        float angleZ = glm::degrees(glm::acos(glm::abs(glm::dot(localZ, gravityDir))));
        if (angleZ < bestAxis.angle)
            bestAxis = {localZ, angleZ};

        const float angleThreshold = 5.0f;
        if (bestAxis.angle < angleThreshold)
        {
            const float angularVelocityThreshold = 0.01f;
            if (glm::length(angularVelocity) < angularVelocityThreshold)
                angularVelocity = glm::vec3(0.0f);
            return;
        }

        glm::vec3 targetAxis = gravityDir;

        float dotProd = glm::dot(bestAxis.axis, gravityDir);
        if (dotProd < 0.0f)
            targetAxis = -gravityDir;
        glm::vec3 rotationAxis = glm::cross(bestAxis.axis, targetAxis);
        if (glm::length(rotationAxis) < 0.001f)
            return; 

        rotationAxis = glm::normalize(rotationAxis);

        float torqueStrength = glm::radians(bestAxis.angle) * 10.0f;

        glm::vec3 correctiveTorque = rotationAxis * torqueStrength;

        float dampingCoefficient = 0.1f;
        glm::vec3 dampingTorque = -angularVelocity * angularDamping;

        glm::vec3 totalTorque = correctiveTorque + dampingTorque;

        AddTorque(totalTorque, ForceMode::Force);
    }

#if EDITOR
#include <imgui.h>

    void Rigidbody::DrawImGui()
    {
        ImGui::Text("Rigidbody Settings");

        if (ImGui::InputFloat("Mass", &mass, 0.1f, 1.0f, "%.3f"))
        {
            if (mass < 0.0001f)
                mass = 0.0001f;
            inverseMass = 1.0f / mass;
            computeInertiaTensor();
        }

        ImGui::SliderFloat("Linear Damping", &linearDamping, 0.0f, 1.0f);
        ImGui::SliderFloat("Angular Damping", &angularDamping, 0.0f, 1.0f);

        ImGui::Checkbox("Enable Friction", &frictionEnabled);
        if (frictionEnabled)
            ImGui::SliderFloat("Friction", &friction, 0.0f, 1.0f);

        ImGui::SliderFloat("Restitution", &restitution, 0.0f, 1.0f);

        if (ImGui::CollapsingHeader("Constraints"))
        {
            ImGui::Checkbox("Freeze Position X", &constraints.freezePositionX);
            ImGui::Checkbox("Freeze Position Y", &constraints.freezePositionY);
            ImGui::Checkbox("Freeze Position Z", &constraints.freezePositionZ);

            ImGui::Checkbox("Freeze Rotation X", &constraints.freezeRotationX);
            ImGui::Checkbox("Freeze Rotation Y", &constraints.freezeRotationY);
            ImGui::Checkbox("Freeze Rotation Z", &constraints.freezeRotationZ);
        }

        if (transform)
        {
            ImGui::Separator();
            ImGui::Text("Transform (readonly):");
            glm::vec3 pos = transform->GetPosition();
            ImGui::Text("Position: %.3f %.3f %.3f", pos.x, pos.y, pos.z);

            glm::quat rot = transform->GetRotation();
            ImGui::Text("Rotation: %.3f %.3f %.3f %.3f", rot.w, rot.x, rot.y, rot.z);

            ImGui::Text("Velocity: %.3f %.3f %.3f", velocity.x, velocity.y, velocity.z);
            ImGui::Text("Angular Velocity: %.3f %.3f %.3f", angularVelocity.x, angularVelocity.y, angularVelocity.z);
        }
    }
#endif


    rapidjson::Value Rigidbody::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {

        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(mass)
        SERIALIZE_FIELD(inverseMass)
        SERIALIZE_FIELD(linearDamping)
        SERIALIZE_FIELD(angularDamping)
        SERIALIZE_FIELD(friction)
        SERIALIZE_FIELD(frictionEnabled)
        SERIALIZE_FIELD(restitution)  
        END_COMPONENT_SERIALIZATION
    }

    void Rigidbody::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(mass)
        DESERIALIZE_VALUE(inverseMass)
        DESERIALIZE_VALUE(linearDamping)
        DESERIALIZE_VALUE(angularDamping)
        DESERIALIZE_VALUE(friction) 
        DESERIALIZE_VALUE(frictionEnabled)
        DESERIALIZE_VALUE(restitution)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void Rigidbody::DeserializeReferencesPass(const rapidjson::Value& Object,
                                              Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        DESERIALIZE_POINTER(transform)
        DESERIALIZE_POINTER(mesh)
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }

} // namespace Engine
