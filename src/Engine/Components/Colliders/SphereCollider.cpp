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

    void SphereCollider::Start() {}

    void SphereCollider::OnDestroy() {}

} // namespace Engine
