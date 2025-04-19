#include "MeshCollider.h"

namespace Engine
{
    MeshCollider::MeshCollider(Transform* transform, bool isTrigger, float scale, Models::Mesh* mesh_p) :
        Collider(transform, isTrigger), _scale(scale), _mesh_p(mesh_p)
    {
        this->colliderType = MESH;
    }

    /*bool MeshCollider::AcceptCollision(ColliderVisitor& visitor)
    {
        visitor.ResolveCollisionMesh(*this);
        return true;
    }*/

    //bool MeshCollider::CheckCollision(const Collider& other)
    //{
    //    if (isStatic || other.IsStatic())
    //        return false;

    //    // TODO: implement collision detection for MeshCollider / remove func

    //    return false;
    //}

    MeshCollider& MeshCollider::operator=(const MeshCollider& other)
    {
        if (this == &other)
            return *this;

        Collider::operator=(other);
        _scale = other._scale;
        _mesh_p = other._mesh_p;

        return *this;
    }

    rapidjson::Value MeshCollider::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {

        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(isTrigger);
        SERIALIZE_FIELD(isStatic);
        SERIALIZE_FIELD(colliderType);
        SERIALIZE_FIELD(_scale);
        END_COMPONENT_SERIALIZATION
    }

    void MeshCollider::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(isTrigger);
        DESERIALIZE_VALUE(isStatic);
        DESERIALIZE_VALUE(colliderType);
        DESERIALIZE_VALUE(_scale);
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void MeshCollider::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                Serialization::ReferenceTable& ReferenceMap)
    {
    }

    float MeshCollider::GetScale() const { return _scale; }
    void MeshCollider::SetScale(float scale) { _scale = scale; }

    Models::Mesh* MeshCollider::GetMesh() const { return _mesh_p; }
    void MeshCollider::SetMesh(Models::Mesh* mesh_p) { _mesh_p = mesh_p; }

    bool MeshCollider::AcceptCollision(ColliderVisitor& visitor)
    {
        visitor.ResolveCollisionMesh(*this); 
        return true;
    }

    void MeshCollider::DrawDebugMesh()
    {
        
    }

    void MeshCollider::Start() 
    { 
        transform = GetOwner()->GetTransform();
    }

    void MeshCollider::OnDestroy() {}

} // namespace Engine
