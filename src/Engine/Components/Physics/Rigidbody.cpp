#include "Rigidbody.h"
#include <algorithm>
#include <glm/gtx/quaternion.hpp>
#include <limits>
#include "Engine/Components/Renderers/ModelRenderer.h"
#include "Engine/EngineObjects/Entity.h"
#include "Engine/EngineObjects/RigidbodyUpdateManager.h"
#include "Serialization/SerializationUtility.h"
#include <iostream>
namespace Engine
{

    Rigidbody::Rigidbody() :
        transform(nullptr), mesh(nullptr), mass(1.0f), inverseMass(1.0f), inertiaTensor(1.0f),
        inverseInertiaTensor(1.0f), velocity(0.0f), angularVelocity(0.0f), linearDamping(0.01f), angularDamping(0.01f),
        friction(0.5f), frictionEnabled(true), restitution(0.3f), accumulatedForce(0.0f), accumulatedTorque(0.0f),
        lastPosition(0.0f), lastRotation(1, 0, 0, 0)
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
        float ix = (1.0f / 12.0f) * mass * (1.0f * 1.0f + 1.0f * 1.0f);
        float iy = ix;
        float iz = ix;

        inertiaTensor = glm::vec3(ix, iy, iz);
        inverseInertiaTensor =
                glm::vec3(ix > 0.0f ? 1.0f / ix : 0.0f, iy > 0.0f ? 1.0f / iy : 0.0f, iz > 0.0f ? 1.0f / iz : 0.0f);
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
            angularVelocity += torque * inverseInertiaTensor;
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

        lastPosition = transform->GetPosition();
        lastRotation = transform->GetRotation();

        glm::vec3 acceleration = accumulatedForce * inverseMass;
        velocity += acceleration * deltaTime;
        velocity *= (1.0f - linearDamping);

        glm::vec3 angularAcceleration = accumulatedTorque * inverseInertiaTensor;
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

        glm::quat deltaRot = glm::quat(0, newAngularVelocity.x, newAngularVelocity.y, newAngularVelocity.z) *
                             transform->GetRotation();
        deltaRot *= 0.5f * deltaTime;
        glm::quat updatedRot = transform->GetRotation() + deltaRot;
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

        if (mesh)
        {
            SnapToGroundFace();
        }
    }

    void Rigidbody::SnapToGroundFace()
    {
        if (!transform || !mesh || mesh->VertexIndices.size() < 3)
            return;

        float minAvgY = std::numeric_limits<float>::max();
        glm::vec3 bestFaceWorldVerts[3];

        for (size_t i = 0; i + 2 < mesh->VertexIndices.size(); i += 3)
        {
            glm::vec3 worldVerts[3];

            for (int j = 0; j < 3; ++j)
            {
                int index = mesh->VertexIndices[i + j];
                const glm::vec3& localPos = mesh->VerticesData[index].Position;
                worldVerts[j] = transform->GetPosition() + transform->GetRotation() * localPos;
            }

            float y0 = worldVerts[0].y;
            float y1 = worldVerts[1].y;
            float y2 = worldVerts[2].y;
            float avgY = (y0 + y1 + y2) / 3.0f;

            float maxDeltaY = std::max({std::abs(y0 - y1), std::abs(y1 - y2), std::abs(y2 - y0)});

            if (maxDeltaY < 0.01f && avgY < minAvgY)
            {
                minAvgY = avgY;
                bestFaceWorldVerts[0] = worldVerts[0];
                bestFaceWorldVerts[1] = worldVerts[1];
                bestFaceWorldVerts[2] = worldVerts[2];
            }
        }

        if (minAvgY == std::numeric_limits<float>::max())
            return;

        float minY = std::min({bestFaceWorldVerts[0].y, bestFaceWorldVerts[1].y, bestFaceWorldVerts[2].y});
        float deltaY = 0.0f - minY;

        glm::vec3 newPosition = transform->GetPosition();
        newPosition.y += deltaY;
        transform->SetPosition(newPosition);

        if (velocity.y < 0.0f)
            velocity.y = 0.0f;
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

        glm::vec3 Iinv_rA_cross_n =
                inverseInertiaTensor * rA_cross_n;
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
        mesh = GetOwner()->GetComponent<ModelRenderer>()->GetModel()->GetMesh(0);
        RigidbodyUpdateManager::GetInstance()->RegisterRigidbody(this);
    }

    void Rigidbody::OnDestroy() 
    { 
        RigidbodyUpdateManager::GetInstance()->UnregisterRigidbodyImmediate(this);
    }



    #if EDITOR
#include <imgui.h>

    void Rigidbody::DrawImGui()
    {
        ImGui::Text("Rigidbody Settings");

        // Mass
        if (ImGui::InputFloat("Mass", &mass, 0.1f, 1.0f, "%.3f"))
        {
            if (mass < 0.0001f)
                mass = 0.0001f; // unikamy zerowej masy
            inverseMass = 1.0f / mass;
            computeInertiaTensor();
        }

        // Linear Damping
        ImGui::SliderFloat("Linear Damping", &linearDamping, 0.0f, 1.0f);

        // Angular Damping
        ImGui::SliderFloat("Angular Damping", &angularDamping, 0.0f, 1.0f);

        // Friction Enabled
        ImGui::Checkbox("Enable Friction", &frictionEnabled);

        // Friction
        if (frictionEnabled)
            ImGui::SliderFloat("Friction", &friction, 0.0f, 1.0f);

        // Restitution (bounciness)
        ImGui::SliderFloat("Restitution", &restitution, 0.0f, 1.0f);

        // Constraints group
        if (ImGui::CollapsingHeader("Constraints"))
        {
            ImGui::Checkbox("Freeze Position X", &constraints.freezePositionX);
            ImGui::Checkbox("Freeze Position Y", &constraints.freezePositionY);
            ImGui::Checkbox("Freeze Position Z", &constraints.freezePositionZ);

            ImGui::Checkbox("Freeze Rotation X", &constraints.freezeRotationX);
            ImGui::Checkbox("Freeze Rotation Y", &constraints.freezeRotationY);
            ImGui::Checkbox("Freeze Rotation Z", &constraints.freezeRotationZ);
        }

        // Transform (readonly)
        if (transform)
        {
            ImGui::Separator();
            ImGui::Text("Transform (readonly):");
            glm::vec3 pos = transform->GetPosition();
            glm::quat rot = transform->GetRotation();

            ImGui::Text("Position: %.3f, %.3f, %.3f", pos.x, pos.y, pos.z);
            ImGui::Text("Rotation (quat): %.3f, %.3f, %.3f, %.3f", rot.w, rot.x, rot.y, rot.z);
        }

        // Mesh selection
        ImGui::Separator();
        ImGui::Text("Mesh:");

        // Za³ó¿my, ¿e masz dostêp do listy meshów w silniku, np:
        // std::vector<Models::Mesh*> availableMeshes = GetAvailableMeshes();

        static int currentMeshIndex = -1;
        std::vector<Models::Mesh*> availableMeshes = /* pobierz listê meshów */;
        std::vector<const char*> meshNames;
        for (auto& m : availableMeshes)
            meshNames.push_back(m->GetName().c_str());

        if (mesh)
        {
            // ZnajdŸ index obecnego mesha w liœcie, jeœli nie znaleziony - -1
            auto it = std::find(availableMeshes.begin(), availableMeshes.end(), mesh);
            currentMeshIndex =
                    (it != availableMeshes.end()) ? static_cast<int>(std::distance(availableMeshes.begin(), it)) : -1;
        }
        else
        {
            currentMeshIndex = -1;
        }

        if (ImGui::Combo("Select Mesh", &currentMeshIndex, meshNames.data(), static_cast<int>(meshNames.size())))
        {
            if (currentMeshIndex >= 0 && currentMeshIndex < (int) availableMeshes.size())
            {
                mesh = availableMeshes[currentMeshIndex];
            }
            else
            {
                mesh = nullptr;
            }
        }
    }
    #endif


    rapidjson::Value Rigidbody::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {

        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(mass)
        SERIALIZE_FIELD(inverseMass)
        SERIALIZE_FIELD(inertiaTensor)
        SERIALIZE_FIELD(inverseInertiaTensor)
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
        DESERIALIZE_VALUE(inertiaTensor)
        DESERIALIZE_VALUE(inverseInertiaTensor)
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
