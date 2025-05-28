#include "Collider.h"
#include "SpatialPartitioning.h"
#include "../Transform.h" // TODO: Fix later. I'm using this way because of indexing problems
#include "Serialization/SerializationUtility.h"
#include "Engine/EngineObjects/CollisionUpdateManager.h"
#include "Shaders/ShaderManager.h"
#include <random>
#include "spdlog/spdlog.h"
#include "Engine/EngineObjects/Entity.h"
#include <iostream>
#include "Engine/Components/BaseMovement/MovementComponent.h"
#include "Engine/EngineObjects/Entity.h"

namespace Engine
{

    Collider::Collider() :
        isTrigger(false), isStatic(false), transform(nullptr), isColliding(false),
        Spatial(&SpatialPartitioning::GetInstance())
    {
        this->colliderVisitor = ColliderVisitor();
        transform = GetOwner()->GetTransform();
#if EDITOR
        SetMaterial(Materials::MaterialManager::GetMaterial("res/materials/Editor/Gizmo.mat"));
#endif
    }

    Collider::~Collider()
    {
        colliderVisitor.~ColliderVisitor(); }

    std::vector<Collider*> Collider::SphereOverlap(glm::vec3& position, float Radius) const
    {
        return SpatialPartitioning::GetInstance().QuerySphere(position, Radius);
    }

    Collider& Collider::operator=(const Collider& Other)
    {
        if (this == &Other)
            return *this;

        isTrigger = Other.isTrigger;
        transform = Other.transform;
        OnCollision = Other.OnCollision;
        OnTrigger = Other.OnTrigger;

        return *this;
    }

    bool Collider::operator==(const Collider& other) const
    {
        return this->GetID() == other.GetID();
    }

    void Collider::Start()
    {
#if EDITOR
        Renderer::Start();
#else
        Component::Start();
#endif
        isColliding = false;
        transform = GetOwner()->GetTransform();
        colliderVisitor.SetCurrentCollider(this);
        Spatial = &SpatialPartitioning::GetInstance();
        Spatial->AddCollider(this);
        CollisionUpdateManager::GetInstance()->RegisterCollider(this);
    }

    void Collider::Update(float DeltaTime)
    {
        if (isStatic)
            return;
        colliderVisitor.ManageCollisions();
    }

    void Collider::OnDestroy()
    {
        Spatial->RemoveCollider(this);
        CollisionUpdateManager::GetInstance()->UnregisterCollider(this);
    }

    // TODO: remove when rigidbody implemented
    float Collider::GetRandomFloat(const float Min, const float Max)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(Min, Max);
        return dis(gen);
    }


} // namespace Engine
