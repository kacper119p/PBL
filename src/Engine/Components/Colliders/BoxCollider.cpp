#include "BoxCollider.h"

#include "Engine/EngineObjects/Entity.h"
#include "Shaders/ShaderManager.h"
#include "Utility/MathUtility.h"
#include "spdlog/spdlog.h"

namespace Engine
{
    BoxCollider::BoxCollider() :
        _width(1.0f), _height(1.0f), _depth(1.0f)
    {
        this->colliderType = BOX;
#if EDITOR
        glGenVertexArrays(1, &Vao);
        glGenBuffers(1, &Vbo);
        glGenBuffers(1, &Ebo);
        UpdateBuffers();
#endif
    }

    BoxCollider::~BoxCollider()
    {
#if EDITOR
        glDeleteVertexArrays(1, &Vao);
        glDeleteBuffers(1, &Vbo);
        glDeleteBuffers(1, &Ebo);
#endif
    }

    void BoxCollider::Start() 
    { 
        Collider::Start();
        transform = GetOwner()->GetTransform();
    }

    PrimitiveMesh* BoxCollider::GetMesh()
    
    {
        mesh = PrimitiveMeshes::GetInstance().GetBoxMesh(transform->GetPosition(), transform->GetRotation(),
                                                              _width, _height, _depth);
        return &mesh;
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

    rapidjson::Value BoxCollider::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {

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
#if EDITOR
        UpdateBuffers();
#endif
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void BoxCollider::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                Serialization::ReferenceTable& ReferenceMap)
    {

        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        DESERIALIZE_POINTER(transform)
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }

    glm::mat3 BoxCollider::CalculateInertiaTensorBody(float mass) const
    {
        float ix = (1.0f / 12.0f) * mass * (_height * _height + _depth * _depth);
        float iy = (1.0f / 12.0f) * mass * (_width * _width + _depth * _depth);
        float iz = (1.0f / 12.0f) * mass * (_width * _width + _height * _height);

        return glm::mat3(ix, 0.0f, 0.0f, 0.0f, iy, 0.0f, 0.0f, 0.0f, iz);
    }

#if EDITOR
    void BoxCollider::DrawDebugMesh(const CameraRenderData& RenderData) const
    {
        Material->GetMainPass().SetUniform("ViewMatrix", RenderData.ViewMatrix);
        Material->GetMainPass().SetUniform("ProjectionMatrix", RenderData.ProjectionMatrix);
        Material->GetMainPass().SetUniform("ObjectToWorldMatrix",
                                           Utility::RemoveScaleMat4(
                                                   GetOwner()->GetTransform()->GetLocalToWorldMatrix()));

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(Vao);
        glDrawElements(GL_LINES, 24 * sizeof(uint32_t), GL_UNSIGNED_INT, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void BoxCollider::Render(const CameraRenderData& RenderData)
    {
        DrawDebugMesh(RenderData);
    }

    void BoxCollider::RenderDepth(const CameraRenderData& RenderData)
    {
        DrawDebugMesh(RenderData);
    }

    void BoxCollider::RenderDirectionalShadows(const CameraRenderData& RenderData)
    {
    }

    void BoxCollider::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                             const glm::mat4* SpaceTransformMatrices)
    {
    }

    void BoxCollider::DrawImGui()
    {
        ImGui::Text("Box Collider");
        ImGui::Separator();
        ImGui::Checkbox("Is Static", &isStatic);

        bool changed = false;

        ImGui::Separator();
        changed |= ImGui::DragFloat("Width", &_width, 0.1f, 0.0f, FLT_MAX, "%.2f");
        changed |= ImGui::DragFloat("Height", &_height, 0.1f, 0.0f, FLT_MAX, "%.2f");
        changed |= ImGui::DragFloat("Depth", &_depth, 0.1f, 0.0f, FLT_MAX, "%.2f");

        if (changed)
        {
            UpdateBuffers();
        }

        ImGui::Separator();
    }

    void BoxCollider::UpdateBuffers()
    {
        const glm::vec3 halfExtents = glm::vec3(_width, _height, _depth) * 0.5f;

        const float vertices[24] = {
                -halfExtents.x, -halfExtents.y, -halfExtents.z,
                halfExtents.x, -halfExtents.y, -halfExtents.z,
                halfExtents.x, halfExtents.y, -halfExtents.z,
                -halfExtents.x, halfExtents.y, -halfExtents.z,
                -halfExtents.x, -halfExtents.y, halfExtents.z,
                halfExtents.x, -halfExtents.y, halfExtents.z,
                halfExtents.x, halfExtents.y, halfExtents.z,
                -halfExtents.x, halfExtents.y, halfExtents.z
        };

        constexpr uint32_t indices[24] = {
                0, 1, 1, 2, 2, 3, 3, 0,
                4, 5, 5, 6, 6, 7, 7, 4,
                0, 4, 1, 5, 2, 6, 3, 7
        };

        glBindVertexArray(Vao);

        glBindBuffer(GL_ARRAY_BUFFER, Vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);
    };
#endif

} // namespace Engine
