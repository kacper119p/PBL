#include "BoxCollider.h"
#include "Shaders/ShaderManager.h"
#include "Engine/EngineObjects/RenderingManager.h"
#include "SpatialPartitioning.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "spdlog/spdlog.h"

namespace Engine
{
    BoxCollider::BoxCollider() : _width(1.0f), _height(1.0f), _depth(1.0f) 
    { 
        this->colliderType = BOX; 
        RenderingManager::GetInstance()->RegisterRenderer(this);
        UpdateManager::GetInstance()->RegisterComponent(this);
    }

    BoxCollider::BoxCollider(Transform* transform, bool isTrigger, float width, float height, float depth) :
        Collider(transform, isTrigger), _width(width), _height(height), _depth(depth)
    {
        this->colliderType = BOX;
        RenderingManager::GetInstance()->RegisterRenderer(this);
        UpdateManager::GetInstance()->RegisterComponent(this);
    }

    bool BoxCollider::AcceptCollision(ColliderVisitor& visitor)
    {
        visitor.ResolveCollisionBox(*this);
        return true;
    }

    BoxCollider& BoxCollider::operator=(const BoxCollider& other)
    {
        if (this == &other)
            return *this;

        Collider::operator=(other);
        _width = other._width;
        _height = other._height;
        _depth = other._depth;

        return *this;
    }

    std::string BoxCollider::loadShaderSource(const char* filePath)
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

    rapidjson::Value BoxCollider::Serialize(rapidjson::Document::AllocatorType& Allocator) const {
    
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(isTrigger);
        SERIALIZE_FIELD(isStatic);
        SERIALIZE_FIELD(colliderType);
        SERIALIZE_FIELD(_width)
        SERIALIZE_FIELD(_height);
        SERIALIZE_FIELD(_depth);
        END_COMPONENT_SERIALIZATION

    }

    void BoxCollider::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap) 
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(isTrigger);
        DESERIALIZE_VALUE(isStatic);
        DESERIALIZE_VALUE(colliderType);
        DESERIALIZE_VALUE(_width);
        DESERIALIZE_VALUE(_height);
        DESERIALIZE_VALUE(_depth);
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void BoxCollider::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                        Serialization::ReferenceTable& ReferenceMap) {}

    float BoxCollider::GetWidth() const { return _width; }
    void BoxCollider::SetWidth(float width) { _width = width; }

    float BoxCollider::GetHeight() const { return _height; }
    void BoxCollider::SetHeight(float height) { _height = height; }

    float BoxCollider::GetDepth() const { return _depth; }
    void BoxCollider::SetDepth(float depth) { _depth = depth; }

    void BoxCollider::DrawDebugMesh(const CameraRenderData& RenderData)
    {
        glm::vec3 halfExtents = glm::vec3(_width, _height, _depth) * 0.5f;

        float vertices[] = {
            -halfExtents.x, -halfExtents.y, -halfExtents.z,
             halfExtents.x, -halfExtents.y, -halfExtents.z,
             halfExtents.x,  halfExtents.y, -halfExtents.z,
            -halfExtents.x,  halfExtents.y, -halfExtents.z,
            -halfExtents.x, -halfExtents.y,  halfExtents.z,
             halfExtents.x, -halfExtents.y,  halfExtents.z,
             halfExtents.x,  halfExtents.y,  halfExtents.z,
            -halfExtents.x,  halfExtents.y,  halfExtents.z
        };

        unsigned int indices[] = {
            0, 1, 1, 2, 2, 3, 3, 0,
            4, 5, 5, 6, 6, 7, 7, 4,
            0, 4, 1, 5, 2, 6, 3, 7
        };

        unsigned int VAO, VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        Shaders::Shader shader = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles("res/shaders/basic/basic.vert", nullptr,
                                      "res/shaders/basic/basic.frag"));

        shader.Use();

        shader.SetUniform("CameraPosition", RenderData.CameraPosition);
        shader.SetUniform("ViewMatrix", RenderData.ViewMatrix);
        shader.SetUniform("ProjectionMatrix", RenderData.ProjectionMatrix);
        shader.SetUniform("ObjectToWorldMatrix", GetOwner()->GetTransform()->GetLocalToWorldMatrix());
        shader.SetUniform("Tint", glm::vec3(0.0f, 5.0f, 0.0f));

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(VAO);
        glDrawElements(GL_LINES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void BoxCollider::Render(const CameraRenderData& RenderData) { 
        BoxCollider::DrawDebugMesh(RenderData);
    }

    void BoxCollider::RenderDepth(const CameraRenderData& RenderData) {}

    void BoxCollider::RenderDirectionalShadows(const CameraRenderData& RenderData) {}

    void BoxCollider::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                             const glm::mat4* SpaceTransformMatrices)
    {}

    void BoxCollider::Start()
    { 
       isColliding = false;
       transform = Component::GetOwner()->GetTransform();
       spdlog::info("BoxCollider start");
       colliderVisitor.SetCurrentCollider(this);
       spatial = &SpatialPartitioning::GetInstance();
       spatial->AddCollider(this);
       currentCellIndex = spatial->GetCellIndex(transform->GetPositionWorldSpace());
       spdlog::info("BoxCollider start, current cell index: {}", currentCellIndex.x);
    }

    void BoxCollider::Update(float deltaTime)
    { 
        // TODO: remove when scriptable fully implemented
        if (currentCellIndex != spatial->GetCellIndex(transform->GetPosition()))
        {
            spatial->RemoveCollider(this);
            spatial->AddCollider(this);
        }
        if (shouldMove)
        {
            glm::vec3 newPosition = transform->GetPosition() - glm::vec3(10.0f, 0.0f, 0.0f) * deltaTime;
            transform->SetPosition(newPosition);
            if (isColliding)
            {
                shouldMove = false;
            }
        }
        // TODO END
        colliderVisitor.ManageCollisions();
    }

    void BoxCollider::OnDestroy() 
    { 
        UpdateManager::GetInstance()->UnregisterComponent(this);
    }

} // namespace Engine
