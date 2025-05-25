#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include "Engine/Components/Transform.h"
#include "Models/Mesh.h"
#include "Engine/Components/Component.h"
#include "Engine/Components/Colliders/ColliderVisitor.h"
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

        void SnapToGroundFace();

        void OnCollision(Rigidbody* other, const glm::vec3& contactPoint, const glm::vec3& contactNormal);
        void OnCollisionStatic(const glm::vec3& contactPoint, const glm::vec3& contactNormal);

        void Interpolate(float alpha);

        void Start() override;
        void OnDestroy() override;

        #if EDITOR
        void DrawImGui() override;
        #endif

        SERIALIZATION_EXPORT_CLASS(Rigidbody)

    public:
        Transform* transform;
        Models::Mesh* mesh;

        float mass;
        float inverseMass;

        glm::vec3 inertiaTensor;
        glm::vec3 inverseInertiaTensor;

        glm::vec3 velocity;
        glm::vec3 angularVelocity;

        float linearDamping;
        float angularDamping;

        float friction;
        bool frictionEnabled;

        float restitution;

        Constraints constraints;

        glm::vec3 accumulatedForce;
        glm::vec3 accumulatedTorque;

        glm::vec3 lastPosition;
        glm::quat lastRotation;

    private:
        void computeInertiaTensor();
       
    };
} // namespace Engine
