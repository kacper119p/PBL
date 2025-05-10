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
#include <random>
namespace Engine
{
    
    Collider::Collider() : isTrigger(false), isStatic(false), transform(nullptr), spatial(&SpatialPartitioning::GetInstance())
    {
        this->colliderVisitor = ConcreteColliderVisitor();
        spatial = &SpatialPartitioning::GetInstance();
        SetMaterial(Materials::MaterialManager::GetMaterial("res/materials/SampleScene/Default.mat"));
        transform = GetOwner()->GetTransform();
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
        Material->GetMainPass().SetUniform("Tint", glm::vec3(.0f, 5.0f, .0f));
        RenderingManager::GetInstance()->RegisterRenderer(this);
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
            glm::vec3 randomRotation(GetRandomFloat(40.0f, 60.0f),
                                     GetRandomFloat(40.0f, 60.0f),
                                     GetRandomFloat(40.0f, 60.0f)
            );

            transform->SetEulerAngles(transform->GetEulerAngles() + randomRotation * deltaTime);
            // TODO END
        }
        
        colliderVisitor.ManageCollisions();
    }

    void Collider::OnDestroy() 
    { 
        CollisionUpdateManager::GetInstance()->UnregisterCollider(this);  
        RenderingManager::GetInstance()->UnregisterRenderer(this);
        spatial->RemoveCollider(this);
    }

    // TODO: remove when rigidbody implemented
    float Collider::GetRandomFloat(float min, float max)
    {
        static std::random_device rd; 
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }


} // namespace Engine
