#include "Collider.h"
#include "SpatialPartitioning.h"
#include "../Transform.h" // TODO: Fix later. I'm using this way because of indexing problems
#include "Serialization/SerializationUtility.h"
#include "spdlog/spdlog.h"
#include "Engine/EngineObjects/CollisionUpdateManager.h"
#include "Shaders/ShaderManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Engine/EngineObjects/RenderingManager.h"

namespace Engine
{
    
    Collider::Collider() : isTrigger(false), transform(nullptr), spatial(&SpatialPartitioning::GetInstance())
    {
        this->colliderVisitor = ConcreteColliderVisitor();
        spatial = &SpatialPartitioning::GetInstance();
        isColliding = false; // TODO: to be removed, just for debug purposes
    }

    Collider::Collider(Transform* transform, bool isTrigger, bool isStatic) 
        :
        isTrigger(isTrigger), isStatic(isStatic), transform(transform), spatial(&SpatialPartitioning::GetInstance())
    {
        this->colliderVisitor = ConcreteColliderVisitor(this);
        colliderType = BOX; // Default type, can be changed in derived classes
        isColliding = false; // TODO: to be removed, just for debug purposes
    }

    Collider::~Collider() 
    { 
    }

    void Collider::SetTrigger(bool isTrigger) { this->isTrigger = isTrigger; }

    bool Collider::IsTrigger() const { return isTrigger; }

    void Collider::SetStatic(bool isStatic) { this->isStatic = isStatic; }

    bool Collider::IsStatic() const { return isStatic; }

    void Collider::SetTransform(Transform* transform) { this->transform = transform; }

    Transform* Collider::GetTransform() const { return transform; }

    void Collider::EmitCollisionEnter(Collider* other) { onCollisionEnter.Invoke(other); }

    void Collider::EmitCollisionExit(Collider* other) { onCollisionExit.Invoke(other); }

    bool Collider::GetCollisionStatus() { return isColliding;}

    Collider& Collider::operator=(const Collider& other)
    {
        if (this == &other)
            return *this;

        isTrigger = other.isTrigger;
        transform = other.transform;
        onCollisionEnter = other.onCollisionEnter;
        onCollisionExit = other.onCollisionExit;

        return *this;
    }

    bool Collider::operator==(const Collider& other) const { return this->GetID() == other.GetID();
    }

    void Collider::RenderDepth(const CameraRenderData& RenderData) {}

    void Collider::Render(const CameraRenderData& RenderData) {}

    void Collider::RenderDirectionalShadows(const CameraRenderData& RenderData) {}

    void Collider::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                const glm::mat4* SpaceTransformMatrices) {}

    std::string Collider::loadShaderSource(const char* filePath)
    {
        std::ifstream shaderFile(filePath);
        std::stringstream shaderStream;

        if (!shaderFile)
        {
            throw std::runtime_error("Failed to open shader file");
        }

        shaderStream << shaderFile.rdbuf();
        return shaderStream.str();
    }

    void Collider::Start()
    {
        isColliding = false;
        transform = Component::GetOwner()->GetTransform();
        colliderVisitor.SetCurrentCollider(this);
        spatial = &SpatialPartitioning::GetInstance();
        spatial->AddCollider(this);
        currentCellIndex = spatial->GetCellIndex(transform->GetPositionWorldSpace());
        CollisionUpdateManager::GetInstance()->RegisterCollider(this);
    }

    void Collider::Update(float deltaTime)
    {
        if (currentCellIndex != spatial->GetCellIndex(transform->GetPosition()))
        {
            spatial->RemoveCollider(this);
            spatial->AddCollider(this);
        }
        if (!isStatic)
        {
            // TODO: remove when rigidbody fully implemented
            glm::vec3 newPosition = transform->GetPosition() + gravity * deltaTime;
            transform->SetPosition(newPosition);
        }
        colliderVisitor.ManageCollisions();
    }

    void Collider::OnDestroy() 
    { 
        CollisionUpdateManager::GetInstance()->UnregisterCollider(this);  
        RenderingManager::GetInstance()->UnregisterRenderer(this);
        spatial->RemoveCollider(this);
    }

} // namespace Engine
