#include "CapsuleCollider.h"
#include "Shaders/ShaderManager.h"
#include "Engine/EngineObjects/RenderingManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "spdlog/spdlog.h"

namespace Engine
{

    CapsuleCollider::CapsuleCollider() : _height(2.0f), _radius(2.0f) {
        this->colliderType = CAPSULE;
        this->shouldMove = false;
        RenderingManager::GetInstance()->RegisterRenderer(this);
        UpdateManager::GetInstance()->RegisterComponent(this);
    }

    CapsuleCollider::CapsuleCollider(Transform* transform, bool isTrigger, float radius, float height) :
        Collider(transform, isTrigger), _radius(radius), _height(height)
    {
        this->colliderType = CAPSULE;
        RenderingManager::GetInstance()->RegisterRenderer(this);
        UpdateManager::GetInstance()->RegisterComponent(this);
    }

    bool CapsuleCollider::AcceptCollision(ColliderVisitor& visitor)
    {
        visitor.ResolveCollisionCapsule(*this);
        return true;
    }

    CapsuleCollider& CapsuleCollider::operator=(const CapsuleCollider& other)
    {
        if (this == &other)
            return *this;

        Collider::operator=(other);
        _radius = other._radius;
        _height = other._height;

        return *this;
    }

    std::string CapsuleCollider::loadShaderSource(const char* filePath)
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

    rapidjson::Value CapsuleCollider::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {

        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(isTrigger);
        SERIALIZE_FIELD(isStatic);
        SERIALIZE_FIELD(colliderType);
        SERIALIZE_FIELD(_radius)
        SERIALIZE_FIELD(_height);
        END_COMPONENT_SERIALIZATION
    }

    void CapsuleCollider::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(isTrigger);
        DESERIALIZE_VALUE(isStatic);
        DESERIALIZE_VALUE(colliderType);
        DESERIALIZE_VALUE(_height);
        DESERIALIZE_VALUE(_radius);
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void CapsuleCollider::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                Serialization::ReferenceTable& ReferenceMap)
    {
    }

    float CapsuleCollider::GetRadius() const { return _radius; }
    void CapsuleCollider::SetRadius(float radius) { _radius = radius; }

    float CapsuleCollider::GetHeight() const { return _height; }
    void CapsuleCollider::SetHeight(float height) { _height = height; }

    void CapsuleCollider::DrawDebugMesh(const CameraRenderData& RenderData)
    {
        const int segments = 16;
        const int rings = 8;
        std::vector<glm::vec3> vertices;
        std::vector<unsigned int> indices;

        float halfHeight = (_height - 2.0f * _radius) * 0.5f;
        float stepTheta = glm::pi<float>() / rings;
        float stepPhi = glm::two_pi<float>() / segments;

        for (int i = 0; i <= rings; ++i)
        {
            for (int j = 0; j <= segments; ++j)
            {
                float theta = i * stepTheta;
                float phi = j * stepPhi;

                float x = _radius * sin(theta) * cos(phi);
                float y = -halfHeight - _radius * cos(theta);
                float z = _radius * sin(theta) * sin(phi);

                vertices.push_back(glm::vec3(x, y, z));
            }
        }

        for (int j = 0; j <= segments; ++j)
        {
            float phi = j * stepPhi;
            float x = _radius * cos(phi);
            float z = _radius * sin(phi);
            vertices.push_back(glm::vec3(x, -halfHeight, z));
            vertices.push_back(glm::vec3(x, halfHeight, z));
        }

        for (int i = 0; i <= rings; ++i)
        {
            for (int j = 0; j <= segments; ++j)
            {
                float theta = i * stepTheta;
                float phi = j * stepPhi;

                float x = _radius * sin(theta) * cos(phi);
                float y = halfHeight + _radius * cos(theta);
                float z = _radius * sin(theta) * sin(phi);

                vertices.push_back(glm::vec3(x, y, z));
            }
        }

        for (size_t i = 0; i + 1 < vertices.size(); ++i)
        {
            indices.push_back((unsigned int) i);
            indices.push_back((unsigned int) (i + 1));
        }

        unsigned int VAO, VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);
        glEnableVertexAttribArray(0);

        Shaders::Shader shader = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("res/shaders/basic/basic.vert", nullptr, "res/shaders/basic/basic.frag"));

        shader.Use();

        shader.SetUniform("CameraPosition", RenderData.CameraPosition);
        shader.SetUniform("ViewMatrix", RenderData.ViewMatrix);
        shader.SetUniform("ProjectionMatrix", RenderData.ProjectionMatrix);
        shader.SetUniform("ObjectToWorldMatrix", GetOwner()->GetTransform()->GetLocalToWorldMatrix());
        shader.SetUniform("Tint", glm::vec3(0.0f, 5.0f, 0.0f));

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(VAO);
        glDrawElements(GL_LINES, (GLsizei) indices.size(), GL_UNSIGNED_INT, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }


    void CapsuleCollider::Render(const CameraRenderData& RenderData) { CapsuleCollider::DrawDebugMesh(RenderData); }

    void CapsuleCollider::RenderDepth(const CameraRenderData& RenderData) {}

    void CapsuleCollider::RenderDirectionalShadows(const CameraRenderData& RenderData) {}

    void CapsuleCollider::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                             const glm::mat4* SpaceTransformMatrices)
    {
    }

    void CapsuleCollider::Start() 
    { 
        isColliding = false;
        transform = GetOwner()->GetTransform();
        colliderVisitor.SetCurrentCollider(this);
    }

    void CapsuleCollider::Update(float deltaTime)
    {

        // TODO: remove when scriptable fully implemented
        if (shouldMove)
        {
            glm::vec3 newPosition = transform->GetPosition() - glm::vec3(.1f, 0.0f, 0.0f) * deltaTime;
            transform->SetPosition(newPosition);
            if (isColliding)
            {
                shouldMove = false;
            }
        }
        // TODO END
        colliderVisitor.ManageCollisions(colliders);
    }

    void CapsuleCollider::OnDestroy() 
    { 
        UpdateManager::GetInstance()->UnregisterComponent(this);
    }

} // namespace Engine
