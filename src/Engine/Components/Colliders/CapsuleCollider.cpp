#include "CapsuleCollider.h"

#include "Engine/EngineObjects/Entity.h"
#include "Shaders/ShaderManager.h"
#include "Utility/MathUtility.h"

#include "spdlog/spdlog.h"


namespace Engine
{

    CapsuleCollider::CapsuleCollider() :
        Height(2.0f), Radius(.5f)
    {
        this->colliderType = CAPSULE;
#if EDITOR
        glGenVertexArrays(1, &Vao);
        glGenBuffers(1, &Vbo);
        glGenBuffers(1, &Ebo);
        UpdateBuffers();
#endif
    }

    CapsuleCollider::~CapsuleCollider()
    {
#if EDITOR
        glDeleteVertexArrays(1, &Vao);
        glDeleteBuffers(1, &Vbo);
        glDeleteBuffers(1, &Ebo);
#endif
    }


    bool CapsuleCollider::AcceptCollision(ColliderVisitor& Visitor)
    {
        Visitor.ResolveCollisionCapsule(*this);
        return true;
    }

    PrimitiveMesh* CapsuleCollider::GetMesh()
    {
        mesh = PrimitiveMeshes::GetInstance().GetCapsuleMesh(transform->GetPosition(), transform->GetRotation(), Radius,
                                                             Height);
        return &mesh;
    }

    CapsuleCollider& CapsuleCollider::operator=(const CapsuleCollider& other)
    {
        if (this == &other)
            return *this;

        Collider::operator=(other);
        Radius = other.Radius;
        Height = other.Height;

        return *this;
    }

    rapidjson::Value CapsuleCollider::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {

        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(isTrigger);
        SERIALIZE_FIELD(isStatic);
        SERIALIZE_FIELD(colliderType);
        SERIALIZE_FIELD(Radius)
        SERIALIZE_FIELD(Height);
        END_COMPONENT_SERIALIZATION
    }

    void CapsuleCollider::DeserializeValuePass(const rapidjson::Value& Object,
                                               Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(isTrigger);
        DESERIALIZE_VALUE(isStatic);
        DESERIALIZE_VALUE(colliderType);
        DESERIALIZE_VALUE(Height);
        DESERIALIZE_VALUE(Radius);
#if EDITOR
        UpdateBuffers();
#endif
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void CapsuleCollider::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                    Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }

    glm::mat3 CapsuleCollider::CalculateInertiaTensorBody(float mass) const
    {
        float sphereVolume = (4.0f / 3.0f) * glm::pi<float>() * Radius * Radius * Radius;
        float cylinderVolume = glm::pi<float>() * Radius * Radius * Height;
        float totalVolume = cylinderVolume + sphereVolume;

        float cyl_mass = mass * (cylinderVolume / totalVolume);
        float sphere_mass = mass * (sphereVolume / totalVolume);

        // Cylinder moment of inertia (o� Y - d�ugo��)
        float I_cyl_y = 0.5f * cyl_mass * Radius * Radius;

        // Cylinder moment in osi X i Z (prostopad�e do osi cylindra)
        float I_cyl_xz = (1.0f / 12.0f) * cyl_mass * (3 * Radius * Radius + Height * Height);

        // Sphere moment of inertia (pe�na kula)
        float I_sphere = (2.0f / 5.0f) * sphere_mass * Radius * Radius;

        // Odleg�o�� �rodka masy p�kuli od �rodka kapsu�y (wzd�u� osi Y)
        float halfHeight = Height / 2.0f;
        float d = halfHeight +
                  (3.0f / 8.0f) * Radius; // �rodek masy p�kuli od jej podstawy, wi�c od �rodka kapsu�y przesuni�cie

        // Zasada Steinera - przesuni�cie momentu bezw�adno�ci p�kuli do �rodka kapsu�y
        float I_sphere_shift = sphere_mass * d * d;

        // Sumujemy momenty dla osi Y (obr�t wzd�u� osi d�ugiej kapsu�y)
        float I_y = I_cyl_y + I_sphere;

        // Sumujemy momenty dla osi X i Z (obr�t prostopad�y do osi kapsu�y)
        float I_xz = I_cyl_xz + I_sphere + I_sphere_shift;

        return glm::mat3(I_xz, 0.0f, 0.0f, 0.0f, I_y, 0.0f, 0.0f, 0.0f, I_xz);
    }


#if EDITOR
    void CapsuleCollider::DrawDebugMesh(const CameraRenderData& RenderData, const Shaders::Shader Shader)
    {
        Shader.SetUniform("ViewMatrix", RenderData.ViewMatrix);
        Shader.SetUniform("ProjectionMatrix", RenderData.ProjectionMatrix);
        Shader.SetUniform("ObjectToWorldMatrix",
                          Utility::RemoveScaleMat4(
                                  GetOwner()->GetTransform()->GetLocalToWorldMatrix()));

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(Vao);
        glDrawElements(GL_LINES, (Rings * 2 + 1) * Segments * 6, GL_UNSIGNED_INT, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }


    void CapsuleCollider::Render(const CameraRenderData& RenderData)
    {
        DrawDebugMesh(RenderData, Material->GetMainPass());
    }

    void CapsuleCollider::RenderDepth(const CameraRenderData& RenderData)
    {
        DrawDebugMesh(RenderData, Material->GetDepthPass());
    }

    void CapsuleCollider::RenderDirectionalShadows(const CameraRenderData& RenderData)
    {
    }

    void CapsuleCollider::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                                 const glm::mat4* SpaceTransformMatrices)
    {
    }

    void CapsuleCollider::UpdateBuffers()
    {
        std::vector<glm::vec3> vertices;
        std::vector<unsigned int> indices;

        const float halfHeight = Height * 0.5f;
        constexpr int hemisphereRings = Rings;
        const int totalRings = Rings * 2 + 1;

        for (int i = 0; i <= totalRings; ++i)
        {
            float phi;

            if (i < hemisphereRings)
            {
                phi = glm::half_pi<float>() * (float(i) / hemisphereRings);
            }
            else if (i > hemisphereRings)
            {
                phi = glm::half_pi<float>() + glm::half_pi<float>() * ((float(i - hemisphereRings)) / hemisphereRings);
            }
            else
            {
                phi = glm::half_pi<float>();
            }

            const float y = std::cos(phi);
            const float r = std::sin(phi);

            float yPos = y * Radius;
            if (i < hemisphereRings)
            {
                yPos += halfHeight;
            }
            else if (i > hemisphereRings)
            {
                yPos -= halfHeight;
            }

            for (int j = 0; j <= Segments; ++j)
            {
                const float theta = glm::two_pi<float>() * static_cast<float>(j) / Segments;
                const float x = std::cos(theta);
                const float z = std::sin(theta);

                glm::vec3 normal = glm::normalize(glm::vec3(x * r, y, z * r));
                glm::vec3 position = normal * Radius + glm::vec3(0, yPos - y * Radius, 0);

                vertices.push_back(position);
            }
        }

        constexpr int ringVertices = Segments + 1;
        for (int i = 0; i < totalRings; ++i)
        {
            for (int j = 0; j < Segments; ++j)
            {
                const int curr = i * ringVertices + j;
                const int next = curr + ringVertices;

                indices.push_back(curr);
                indices.push_back(next);
                indices.push_back(curr + 1);

                indices.push_back(curr + 1);
                indices.push_back(next);
                indices.push_back(next + 1);
            }
        }

        glBindVertexArray(Vao);

        glBindBuffer(GL_ARRAY_BUFFER, Vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3)), vertices.data(),
                     GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
                     indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);
        glEnableVertexAttribArray(0);
    }

    void CapsuleCollider::DrawImGui()
    {
        ImGui::Text("Capsule Collider");
        ImGui::Separator();
        ImGui::Checkbox("Is Static", &isStatic);

        bool changed = false;

        changed |= ImGui::DragFloat("Radius", &Radius, 0.1f, 0.0f, FLT_MAX, "%.2f");
        changed |= ImGui::DragFloat("Height", &Height, 0.1f, 0.0f, FLT_MAX, "%.2f");

        if (changed)
        {
            UpdateBuffers();
        }

        ImGui::Separator();
    }
#endif

} // namespace Engine
