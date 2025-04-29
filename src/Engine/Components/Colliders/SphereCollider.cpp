#include "SphereCollider.h"
#include "Shaders/ShaderManager.h"
#include "Engine/EngineObjects/RenderingManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "spdlog/spdlog.h"

namespace Engine
{
    SphereCollider::SphereCollider() : radius(1.0f)
    {
        this->colliderType = SPHERE;
        this->shouldMove = false;
        RenderingManager::GetInstance()->RegisterRenderer(this);
        UpdateManager::GetInstance()->RegisterComponent(this);
    }

    SphereCollider::SphereCollider(Transform* transform, bool isTrigger, float radius) :
        Collider(transform, isTrigger), radius(radius)
    {
        this->colliderType = SPHERE;
    }

    bool SphereCollider::AcceptCollision(ColliderVisitor& visitor)
    {
        visitor.ResolveCollisionSphere(*this);
        return true;
    }

    //bool SphereCollider::CheckCollision(const Collider& other)
    //{
    //    if (isStatic || other.IsStatic())
    //        return false;

    //    // TODO: implement collision detection for SphereCollider / remove func

    //    return false;
    //}

    SphereCollider& SphereCollider::operator=(const SphereCollider& other)
    {
        if (this == &other)
            return *this;

        Collider::operator=(other);
        radius = other.radius;

        return *this;
    }

    rapidjson::Value SphereCollider::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {

        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(isTrigger);
        SERIALIZE_FIELD(isStatic);
        SERIALIZE_FIELD(colliderType);
        SERIALIZE_FIELD(radius);
        END_COMPONENT_SERIALIZATION
    }

    void SphereCollider::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(isTrigger);
        DESERIALIZE_VALUE(isStatic);
        DESERIALIZE_VALUE(colliderType);
        DESERIALIZE_VALUE(radius);
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void SphereCollider::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                Serialization::ReferenceTable& ReferenceMap)
    {
    }

    float SphereCollider::GetRadius() const { return radius; }
    void SphereCollider::SetRadius(float radius) { this->radius = radius; }

    std::string SphereCollider::loadShaderSource(const char* filePath)
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

    void SphereCollider::DrawDebugMesh(const CameraRenderData& RenderData)
    {
        const int latitudeSegments = 16;
        const int longitudeSegments = 16;

        std::vector<glm::vec3> vertices;
        std::vector<unsigned int> indices;

        for (int lat = 0; lat <= latitudeSegments; ++lat)
        {
            float theta = glm::pi<float>() * float(lat) / latitudeSegments;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            for (int lon = 0; lon <= longitudeSegments; ++lon)
            {
                float phi = glm::two_pi<float>() * float(lon) / longitudeSegments;
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);

                float x = radius * sinTheta * cosPhi;
                float y = radius * cosTheta;
                float z = radius * sinTheta * sinPhi;

                vertices.emplace_back(x, y, z);
            }
        }

        for (int lat = 0; lat < latitudeSegments; ++lat)
        {
            for (int lon = 0; lon < longitudeSegments; ++lon)
            {
                int current = lat * (longitudeSegments + 1) + lon;
                int next = current + longitudeSegments + 1;

                // lines for latitudinal circle
                indices.push_back(current);
                indices.push_back(current + 1);

                // lines for longitudinal circle
                indices.push_back(current);
                indices.push_back(next);
            }
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
        shader.SetUniform("Tint", glm::vec3(0.0f, 0.0f, 255.0f));

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(VAO);
        glDrawElements(GL_LINES, (GLsizei) indices.size(), GL_UNSIGNED_INT, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }


    void SphereCollider::Render(const CameraRenderData& RenderData) { SphereCollider::DrawDebugMesh(RenderData); }

    void SphereCollider::RenderDepth(const CameraRenderData& RenderData) {}

    void SphereCollider::RenderDirectionalShadows(const CameraRenderData& RenderData) {}

    void SphereCollider::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                             const glm::mat4* SpaceTransformMatrices)
    {
    }


    void SphereCollider::Start() 
    {
        transform = GetOwner()->GetTransform();
    }

    void SphereCollider::Update(float deltaTime)
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

    void SphereCollider::OnDestroy() {
        RenderingManager::GetInstance()->UnregisterRenderer(this);
        UpdateManager::GetInstance()->UnregisterComponent(this);
    }

} // namespace Engine
