#include "CapsuleCollider.h"

namespace Engine
{
    CapsuleCollider::CapsuleCollider(Transform* transform, bool isTrigger, float radius, float height) :
        Collider(transform, isTrigger), _radius(radius), _height(height)
    {
        this->colliderType = CAPSULE;
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

    void CapsuleCollider::DrawDebugMesh()
    {
        const int segments = 16;
        const int rings = 8;
        const float radius = _radius;
        const float halfHeight = _height * 0.5f - radius;

        glColor3f(0.0f, 1.0f, 0.0f);

        glBegin(GL_LINES);
        for (int i = 0; i < segments; ++i)
        {
            float theta1 = (2.0f * glm::pi<float>() * i) / segments;
            float theta2 = (2.0f * glm::pi<float>() * (i + 1)) / segments;

            float x1 = radius * cos(theta1);
            float z1 = radius * sin(theta1);
            float x2 = radius * cos(theta2);
            float z2 = radius * sin(theta2);

            glVertex3f(x1, -halfHeight, z1);
            glVertex3f(x2, -halfHeight, z2);

            glVertex3f(x1, halfHeight, z1);
            glVertex3f(x2, halfHeight, z2);

            glVertex3f(x1, -halfHeight, z1);
            glVertex3f(x1, halfHeight, z1);
        }
        glEnd();

        for (int i = 0; i <= rings; ++i)
        {
            float phi1 = (glm::pi<float>() * i) / rings;
            float phi2 = (glm::pi<float>() * (i + 1)) / rings;

            glBegin(GL_LINES);
            for (int j = 0; j < segments; ++j)
            {
                float theta1 = (2.0f * glm::pi<float>() * j) / segments;
                float theta2 = (2.0f * glm::pi<float>() * (j + 1)) / segments;

                float x1 = radius * sin(phi1) * cos(theta1);
                float y1 = -halfHeight - radius * cos(phi1);
                float z1 = radius * sin(phi1) * sin(theta1);

                float x2 = radius * sin(phi2) * cos(theta1);
                float y2 = -halfHeight - radius * cos(phi2);
                float z2 = radius * sin(phi2) * sin(theta1);

                float x3 = radius * sin(phi1) * cos(theta2);
                float y3 = -halfHeight - radius * cos(phi1);
                float z3 = radius * sin(phi1) * sin(theta2);

                glVertex3f(x1, y1, z1);
                glVertex3f(x2, y2, z2);

                glVertex3f(x1, y1, z1);
                glVertex3f(x3, y3, z3);

                x1 = radius * sin(phi1) * cos(theta1);
                y1 = halfHeight + radius * cos(phi1);
                z1 = radius * sin(phi1) * sin(theta1);

                x2 = radius * sin(phi2) * cos(theta1);
                y2 = halfHeight + radius * cos(phi2);
                z2 = radius * sin(phi2) * sin(theta1);

                x3 = radius * sin(phi1) * cos(theta2);
                y3 = halfHeight + radius * cos(phi1);
                z3 = radius * sin(phi1) * sin(theta2);

                glVertex3f(x1, y1, z1);
                glVertex3f(x2, y2, z2);

                glVertex3f(x1, y1, z1);
                glVertex3f(x3, y3, z3);
            }
            glEnd();
        }
    }

    void CapsuleCollider::Start() 
    { 
        transform = GetOwner()->GetTransform();
    }

    void CapsuleCollider::OnDestroy() 
    {
    
    }

} // namespace Engine
