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
        // foreach loop doesn't work properly.
        for (int i = 0; i < Children.size(); ++i) // NOLINT(*-loop-convert)
        {
            delete Children[i]->GetOwner();
        }
    }

    void Transform::SetParent(Transform* InParent)
    {
        if (Parent)
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
        Child->Parent->RemoveChild(Child);
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
        START_OBJECT_SERIALIZATION
        object.AddMember("position", Serialization::Serialize(Position, Allocator), Allocator);
        object.AddMember("eulerAngles", Serialization::Serialize(EulerAngles, Allocator), Allocator);
        object.AddMember("scale", Serialization::Serialize(Scale, Allocator), Allocator);
        //TODO Children Serialization
        END_OBJECT_SERIALIZATION
    }

    void Transform::DeserializeValuePass(const rapidjson::Value& Object, Serialization::ReferenceTable& ReferenceMap)
    {
        START_OBJECT_DESERIALIZATION_VALUE_PASS
        Serialization::Deserialize(Object["position"], Position);
        Serialization::Deserialize(Object["eulerAngles"], EulerAngles);
        Serialization::Deserialize(Object["scale"], Scale);
        END_OBJECT_DESERIALIZATION_VALUE_PASS
    }

    void Transform::DeserializeReferencesPass(const rapidjson::Value& Object,
                                              Serialization::ReferenceTable& ReferenceMap)
    {
        START_OBJECT_DESERIALIZATION_REFERENCES_PASS
        //TODO Children Deserialization
        END_OBJECT_DESERIALIZATION_REFERENCES_PASS
    }
}
