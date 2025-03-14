#include "Transform.h"
#include "glm/gtx/euler_angles.hpp"

namespace Engine
{
    Transform::~Transform()
    {
        if (Parent)
        {
            Parent->RemoveChild(this);
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
}
