#include "RectTransform.h"

#include "UiElement.h"
#include "Engine/EngineObjects/Entity.h"
#include "Serialization/SerializationUtility.h"
#include "glm/gtx/euler_angles.hpp"

namespace Engine::Ui
{
    RectTransform::~RectTransform()
    {
        if (Parent)
        {
            Parent->RemoveChild(this);
        }
        for (int i = Children.size() - 1; i >= 0; --i)
        {
            const UiElement* owner = Children[i]->GetOwner();
            delete owner;
        }
    }

    void RectTransform::SetParent(RectTransform* InParent)
    {
        if (Parent)
        {
            Parent->RemoveChild(this);
        }
        Parent = InParent;
        Parent->AddChild(this);
        MarkDirty();
    }


    void RectTransform::AddChild(RectTransform* Child)
    {
        if (Child->Parent != nullptr)
        {
            Child->Parent->RemoveChild(Child);
        }
        Child->Parent = this;
        Child->MarkDirty();
        Children.push_back(Child);
    }

    void RectTransform::RemoveChild(RectTransform* Child)
    {
        if (std::erase(Children, Child))
        {
            Child->Parent = nullptr;
            Child->MarkDirty();
        }
    }

    const std::vector<RectTransform*>& RectTransform::GetChildren()
    {
        return Children;
    }

    const glm::mat4& RectTransform::GetLocalMatrix()
    {
        UpdateMatrices();
        return LocalMatrix;
    }

    const glm::mat4& RectTransform::GetLocalToWorldMatrix()
    {
        UpdateMatrices();
        return LocalToWorldMatrix;
    }

    const glm::mat4& RectTransform::GetLocalToWorldMatrixNoScale()
    {
        UpdateMatrices();
        return LocalToWorldMatrixNoScale;
    }

    void RectTransform::MarkDirty()
    {
        IsDirty = true;
        for (RectTransform* child : Children)
        {
            child->MarkDirty();
        }
    }

    void RectTransform::UpdateMatrices()
    {
        if (!IsDirty)
        {
            return;
        }
        LocalToWorldMatrixNoScale = glm::mat4(1.0f);

        LocalToWorldMatrixNoScale = glm::translate(LocalToWorldMatrixNoScale, Position);

        LocalToWorldMatrixNoScale = glm::rotate(LocalToWorldMatrixNoScale, glm::radians(Rotation),
                                                glm::vec3(0.0f, 0.0f, 1.0f));

        LocalMatrix = glm::scale(LocalToWorldMatrixNoScale, glm::vec3(Size, 1.0f));

        if (Parent != nullptr)
        {
            LocalToWorldMatrixNoScale = Parent->GetLocalToWorldMatrixNoScale() * LocalToWorldMatrixNoScale;
            LocalToWorldMatrix = Parent->GetLocalToWorldMatrixNoScale() * LocalMatrix;
        }
        else
        {
            LocalToWorldMatrix = LocalMatrix;
        }

        IsDirty = false;
    }
}
