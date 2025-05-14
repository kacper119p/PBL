#include "Collider.h"
#include "SpatialPartitioning.h"
#include "../Transform.h" // TODO: Fix later. I'm using this way because of indexing problems
#include "Serialization/SerializationUtility.h"
#include "Engine/EngineObjects/CollisionUpdateManager.h"
#include "Shaders/ShaderManager.h"
#include <random>

#include "Engine/EngineObjects/Entity.h"

namespace Engine
{

    Collider::Collider() :
        isTrigger(false), isStatic(false), transform(nullptr), isColliding(false),
        Spatial(&SpatialPartitioning::GetInstance())
    {
        this->colliderVisitor = ColliderVisitor();
        Spatial = &SpatialPartitioning::GetInstance();
        transform = GetOwner()->GetTransform();
#if EDITOR
        SetMaterial(Materials::MaterialManager::GetMaterial("res/materials/Editor/Gizmo.mat"));
#endif
    }

    Collider::~Collider()
    {
        Spatial->RemoveCollider(this);
        CollisionUpdateManager::GetInstance()->UnregisterCollider(this);
    }

    Collider& Collider::operator=(const Collider& Other)
    {
        if (this == &Other)
            return *this;

        isTrigger = Other.isTrigger;
        transform = Other.transform;
        onCollisionEnter = Other.onCollisionEnter;
        onCollisionExit = Other.onCollisionExit;

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
        CurrentCellIndex = Spatial->GetCellIndex(transform->GetPositionWorldSpace());
        CollisionUpdateManager::GetInstance()->RegisterCollider(this);
    }

    void Collider::Update(float DeltaTime)
    {
        if (CurrentCellIndex != Spatial->GetCellIndex(transform->GetPosition()))
        {
            Spatial->RemoveCollider(this);
            Spatial->AddCollider(this);
        }
        if (!isStatic)
        {
            // TODO: remove when rigidbody fully implemented
            const glm::vec3 newPosition = transform->GetPosition() + Gravity * DeltaTime;
            transform->SetPosition(newPosition);
            const glm::vec3 randomRotation(GetRandomFloat(40.0f, 60.0f),
                                           GetRandomFloat(40.0f, 60.0f),
                                           GetRandomFloat(40.0f, 60.0f)
                    );

            transform->SetEulerAngles(transform->GetEulerAngles() + randomRotation * DeltaTime);
            // TODO END
        }

        colliderVisitor.ManageCollisions();
    }

    void Collider::OnDestroy()
    {
        CollisionUpdateManager::GetInstance()->UnregisterCollider(this);
        Spatial->RemoveCollider(this);
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
