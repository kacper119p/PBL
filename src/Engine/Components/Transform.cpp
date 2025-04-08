#include "Transform.h"

#include "Engine/EngineObjects/Entity.h"
#include "Serialization/SerializationUtility.h"
#include "glm/gtx/euler_angles.hpp"

namespace Engine
{
    Transform::~Transform()
    {
        if (Parent)
        {
            Parent->RemoveChild(this);
        }
        for (int i = Children.size() - 1; i >= 0; --i)
        {
            const Entity* owner = Children[i]->GetOwner();
            delete owner;
        }
    }

    void Transform::SetParent(Transform* InParent)
    {
        if (Parent != nullptr)
        {
            Parent->RemoveChild(this);
        }
        Parent = InParent;
        Parent->AddChild(this);
        MarkDirty();
    }

    Transform* Transform::GetParent() const
    {
        return Parent;
    }

    void Transform::AddChild(Transform* Child)
    {
        if (Child->Parent != nullptr)
        {
            Child->Parent->RemoveChild(Child);
        }
        Child->Parent = this;
        Child->MarkDirty();
        Children.push_back(Child);
    }

    void Transform::RemoveChild(Transform* Child)
    {
        if (std::erase(Children, Child))
        {
            Child->Parent = nullptr;
            Child->MarkDirty();
        }
    }

    const std::vector<Transform*>& Transform::GetChildren()
    {
        return Children;
    }

    const glm::mat4& Transform::GetLocalMatrix()
    {
        UpdateMatrices();
        return LocalMatrix;
    }

    const glm::mat4& Transform::GetLocalToWorldMatrix()
    {
        UpdateMatrices();
        return LocalToWorldMatrix;
    }

    void Transform::MarkDirty()
    {
        IsDirty = true;
        for (Transform* child : Children)
        {
            child->MarkDirty();
        }
    }

    void Transform::UpdateMatrices()
    {
        if (!IsDirty)
        {
            return;
        }
        LocalMatrix = glm::mat4(1.0f);

        LocalMatrix = glm::translate(LocalMatrix, Position);

        LocalMatrix *= glm::eulerAngleXYZ(glm::radians(EulerAngles.x),
                                          glm::radians(EulerAngles.y),
                                          glm::radians(EulerAngles.z));

        LocalMatrix = glm::scale(LocalMatrix, Scale);

        if (Parent)
        {
            LocalToWorldMatrix = Parent->GetLocalToWorldMatrix() * LocalMatrix;
        }
        else
        {
            LocalToWorldMatrix = LocalMatrix;
        }

        IsDirty = false;
    }

    rapidjson::Value Transform::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(Position)
        SERIALIZE_FIELD(EulerAngles)
        SERIALIZE_FIELD(Scale)
        SERIALIZE_FIELD(Children)
        SERIALIZE_FIELD(Parent)
        END_COMPONENT_SERIALIZATION
    }

    void Transform::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(Position);
        DESERIALIZE_VALUE(EulerAngles);
        DESERIALIZE_VALUE(Scale);
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void Transform::DeserializeReferencesPass(const rapidjson::Value& Object,
                                              Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        DESERIALIZE_POINTER(Children)
        DESERIALIZE_POINTER(Parent)
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
}
