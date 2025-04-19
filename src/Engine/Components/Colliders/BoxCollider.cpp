#include "BoxCollider.h"
#include "Serialization/SerializationUtility.h"

namespace Engine
{
    BoxCollider::BoxCollider(Transform* transform, bool isTrigger, float width, float height, float depth) :
        Collider(transform, isTrigger), _width(width), _height(height), _depth(depth)
    {
        this->colliderType = BOX;
    }

    bool BoxCollider::AcceptCollision(ColliderVisitor& visitor)
    {
        visitor.ResolveCollisionBox(*this);
        return true;
    }

    //bool BoxCollider::CheckCollision(const Collider& other)
    //{
    //    if (isStatic || other.IsStatic()) 
    //        return false;

    //    // TODO: implement collision detection for BoxCollider / remove func

    //    return false;
    //}

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

    void BoxCollider::DrawDebugMesh()
    {
        glm::vec3 halfExtents = glm::vec3(_width, _height, _depth) * 0.5f;

        glm::vec3 vertices[8] = {
                {-halfExtents.x, -halfExtents.y, -halfExtents.z},
                {halfExtents.x, -halfExtents.y, -halfExtents.z},
                {halfExtents.x, halfExtents.y, -halfExtents.z},
                {-halfExtents.x, halfExtents.y, -halfExtents.z},
                {-halfExtents.x, -halfExtents.y, halfExtents.z},
                {halfExtents.x, -halfExtents.y, halfExtents.z},
                {halfExtents.x, halfExtents.y, halfExtents.z},
                {-halfExtents.x, halfExtents.y, halfExtents.z}
        };

        unsigned int indices[] = {
                0, 1, 1, 2, 2, 3, 3, 0,
                4, 5, 5, 6, 6, 7, 7, 4,
                0, 4, 1, 5, 2, 6, 3, 7 
        };

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glColor3f(0.0f, 1.0f, 0.0f);

        glBegin(GL_LINES);
        for (unsigned int i = 0; i < sizeof(indices) / sizeof(indices[0]); i += 2)
        {
            glm::vec3 start = vertices[indices[i]];
            glm::vec3 end = vertices[indices[i + 1]];

            glVertex3f(start.x, start.y, start.z);
            glVertex3f(end.x, end.y, end.z);
        }
        glEnd();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void BoxCollider::Start()
    { 
       transform = GetOwner()->GetTransform();
    }

    void BoxCollider::OnDestroy() 
    {

    }

} // namespace Engine
