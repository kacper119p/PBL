#include "SphereCollider.h"

#include "Engine/EngineObjects/Entity.h"
#include "Shaders/ShaderManager.h"
#include "Utility/MathUtility.h"
#include "spdlog/spdlog.h"

namespace Engine
{
    SphereCollider::SphereCollider() :
        radius(1.0f)
    {
        this->colliderType = SPHERE;

#if EDITOR
        glGenVertexArrays(1, &Vao);
        glGenBuffers(1, &Vbo);
        glGenBuffers(1, &Ebo);
        UpdateBuffers();
#endif

    }

    SphereCollider::~SphereCollider()
    {
#if EDITOR
        glDeleteVertexArrays(1, &Vao);
        glDeleteBuffers(1, &Vbo);
        glDeleteBuffers(1, &Ebo);
#endif

    }

    bool SphereCollider::AcceptCollision(ColliderVisitor& visitor)
    {
        visitor.ResolveCollisionSphere(*this);
        return true;
    }


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

    void SphereCollider::DeserializeValuePass(const rapidjson::Value& Object,
                                              Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(isTrigger);
        DESERIALIZE_VALUE(isStatic);
        DESERIALIZE_VALUE(colliderType);
        DESERIALIZE_VALUE(radius);
#if EDITOR
        UpdateBuffers();
#endif
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void SphereCollider::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                   Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        DESERIALIZE_POINTER(transform)
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }

    glm::mat3 SphereCollider::CalculateInertiaTensor(float mass) const
    {
        float I = 0.4f * mass * radius * radius;
        return glm::mat3(I, 0.0f, 0.0f, 0.0f, I, 0.0f, 0.0f, 0.0f, I);
    }

#if EDITOR
    void SphereCollider::DrawDebugMesh(const CameraRenderData& RenderData)
    {
        Material->GetMainPass().SetUniform("ViewMatrix", RenderData.ViewMatrix);
        Material->GetMainPass().SetUniform("ProjectionMatrix", RenderData.ProjectionMatrix);
        Material->GetMainPass().SetUniform("ObjectToWorldMatrix",
                                           Utility::RemoveScaleMat4(
                                                   GetOwner()->GetTransform()->GetLocalToWorldMatrix()));

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(Vao);
        glDrawElements(GL_LINES, static_cast<GLsizei>(LongitudeSegments * LatitudeSegments * 4), GL_UNSIGNED_INT, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void SphereCollider::Render(const CameraRenderData& RenderData)
    {
        DrawDebugMesh(RenderData);
    }

    void SphereCollider::RenderDepth(const CameraRenderData& RenderData)
    {
        DrawDebugMesh(RenderData);
    }

    void SphereCollider::RenderDirectionalShadows(const CameraRenderData& RenderData)
    {
    }

    void SphereCollider::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                                const glm::mat4* SpaceTransformMatrices)
    {
    }

    void SphereCollider::DrawImGui()
    {
        ImGui::Text("Sphere Collider");
        ImGui::Separator();
        ImGui::Checkbox("Is Trigger", &isTrigger);
        ImGui::Checkbox("Is Static", &isStatic);

        bool changed = false;

        changed |= ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, FLT_MAX, "%.2f");
        ImGui::Separator();

        if (changed)
        {
            UpdateBuffers();
        }
    }

    void SphereCollider::UpdateBuffers()
    {
        std::vector<glm::vec3> vertices;
        std::vector<uint32_t> indices;

        for (int lat = 0; lat <= LatitudeSegments; ++lat)
        {
            const float theta = glm::pi<float>() * float(lat) / LatitudeSegments;
            const float sinTheta = sin(theta);
            const float cosTheta = cos(theta);

            for (int lon = 0; lon <= LongitudeSegments; ++lon)
            {
                const float phi = glm::two_pi<float>() * float(lon) / LongitudeSegments;
                const float sinPhi = sin(phi);
                const float cosPhi = cos(phi);

                float x = radius * sinTheta * cosPhi;
                float y = radius * cosTheta;
                float z = radius * sinTheta * sinPhi;

                vertices.emplace_back(x, y, z);
            }
        }

        for (int lat = 0; lat < LatitudeSegments; ++lat)
        {
            for (int lon = 0; lon < LongitudeSegments; ++lon)
            {
                const int current = lat * (LongitudeSegments + 1) + lon;
                const int next = current + LongitudeSegments + 1;

                indices.push_back(current);
                indices.push_back(current + 1);

                indices.push_back(current);
                indices.push_back(next);
            }
        }

        glBindVertexArray(Vao);

        glBindBuffer(GL_ARRAY_BUFFER, Vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3)), vertices.data(),
                     GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(uint32_t)),
                     indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);
        glEnableVertexAttribArray(0);
    }
#endif

} // namespace Engine
