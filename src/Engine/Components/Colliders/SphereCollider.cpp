#include "SphereCollider.h"


namespace Engine
{
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

    void SphereCollider::DrawDebugMesh(const CameraRenderData& RenderData)
    {
        const int segments = 16;
        const int rings = 16;
        const float radius = this->radius;

        glColor3f(0.0f, 1.0f, 0.0f);

        glBegin(GL_LINES);
        for (int i = 0; i <= rings; ++i)
        {
            float phi = glm::pi<float>() * i / rings;
            float y = radius * cos(phi);
            float ringRadius = radius * sin(phi);

            for (int j = 0; j < segments; ++j)
            {
                float theta1 = 2.0f * glm::pi<float>() * j / segments;
                float theta2 = 2.0f * glm::pi<float>() * (j + 1) / segments;

                float x1 = ringRadius * cos(theta1);
                float z1 = ringRadius * sin(theta1);

                float x2 = ringRadius * cos(theta2);
                float z2 = ringRadius * sin(theta2);

                glVertex3f(x1, y, z1);
                glVertex3f(x2, y, z2);
            }
        }
        glEnd();

        glBegin(GL_LINES);
        for (int j = 0; j < segments; ++j)
        {
            float theta = 2.0f * glm::pi<float>() * j / segments;

            for (int i = 0; i < rings; ++i)
            {
                float phi1 = glm::pi<float>() * i / rings;
                float phi2 = glm::pi<float>() * (i + 1) / rings;

                float x1 = radius * sin(phi1) * cos(theta);
                float y1 = radius * cos(phi1);
                float z1 = radius * sin(phi1) * sin(theta);

                float x2 = radius * sin(phi2) * cos(theta);
                float y2 = radius * cos(phi2);
                float z2 = radius * sin(phi2) * sin(theta);

                glVertex3f(x1, y1, z1);
                glVertex3f(x2, y2, z2);
            }
        }
        glEnd();
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

    void SphereCollider::OnDestroy() {}

} // namespace Engine
