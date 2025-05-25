#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include "Engine/Components/Transform.h"
#include "Models/Mesh.h"
#include "Engine/Components/Component.h"
#include "Engine/Components/Colliders/ColliderVisitor.h"
#include "Engine/Components/Colliders/PrimitiveMeshes.h"
namespace Engine
{

    enum class ForceMode
    {
        Force,
        Impulse
    };

    struct Constraints
    {
        bool freezePositionX = false;
        bool freezePositionY = false;
        bool freezePositionZ = false;
        bool freezeRotationX = false;
        bool freezeRotationY = false;
        bool freezeRotationZ = false;
    };

    class Rigidbody : public Component
    {
    public:
        Rigidbody();

        void SetMass(float m);

        void AddForce(const glm::vec3& force, ForceMode mode = ForceMode::Force);
        void AddTorque(const glm::vec3& torque, ForceMode mode = ForceMode::Force);

        void ApplyGravity(const glm::vec3& gravity);

        void Update(float deltaTime);

        void OnCollision(Rigidbody* other, const glm::vec3& contactPoint, const glm::vec3& contactNormal);
        void OnCollisionStatic(const glm::vec3& contactPoint, const glm::vec3& contactNormal);

        void Interpolate(float alpha);

        void SetLastCollisionNormal(const glm::vec3& normal);

        void Start() override;
        void OnDestroy() override;

        #if EDITOR
        void DrawImGui() override;
        #endif

        SERIALIZATION_EXPORT_CLASS(Rigidbody)

    public:
        Transform* transform;
        PrimitiveMesh* mesh;

        float mass;
        float inverseMass;

        glm::mat3 inertiaTensor;
        glm::mat3 inverseInertiaTensor;

        glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f); // default gravity vector
        glm::vec3 velocity;
        glm::vec3 angularVelocity;

        float linearDamping;
        float angularDamping;

        float friction;
        bool frictionEnabled;

        float restitution;

        float alignmentStrength = 5.0f;
        Constraints constraints;

        glm::vec3 accumulatedForce;
        glm::vec3 accumulatedTorque;

        glm::vec3 lastPosition;
        glm::quat lastRotation;

    private:
        void computeInertiaTensor();
        glm::vec3 lastCollisionNormal; // przechowuje normalê kolizji ostatniej kolizji
        float collisionNormalTimeout; // jak d³ugo normalna jest wa¿na (np. 1s)
        float collisionNormalTimer; // odlicza czas od ostatniej kolizji
        void TryAlignToCollisionNormal(float deltaTime);
        void QuaternionToAxisAngle(const glm::quat& q, glm::vec3& out_axis, float& out_angle);
        void ApplyUprightStabilization(float deltaTime);
        void ComputeGravityTorqueFromVertices();
    };
} // namespace Engine
