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

    glm::vec3 Transform::GetForward() const
    {
        glm::vec3 forward = -glm::vec3(LocalToWorldMatrix[2]);
        return glm::normalize(forward);
    }

    glm::vec3 Transform::GetRight() const
    {
        glm::vec3 right = glm::vec3(LocalToWorldMatrix[0]);
        return glm::normalize(right);
    }

    glm::vec3 Transform::GetUp() const
    {
        glm::vec3 up = glm::vec3(LocalToWorldMatrix[1]);
        return glm::normalize(up);
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

#if EDITOR
    void Transform::DrawImGui()
    {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Editable Position
            if (ImGui::DragFloat3("Position", glm::value_ptr(Position), 0.1f))
            {
                MarkDirty();
            }

            // Editable Euler Angles
            if (ImGui::DragFloat3("Rotation (Euler)", glm::value_ptr(EulerAngles), 0.5f))
            {
                MarkDirty();
            }

            // Editable Scale
            if (ImGui::DragFloat3("Scale", glm::value_ptr(Scale), 0.1f))
            {
                MarkDirty();
            }

            // Update matrices if dirty
            if (IsDirty)
            {
                UpdateMatrices();
            }

            // Display Local Matrix
            if (ImGui::TreeNode("Local Matrix"))
            {
                const glm::mat4& local = GetLocalMatrix();
                for (int row = 0; row < 4; ++row)
                {
                    ImGui::Text("%.3f %.3f %.3f %.3f", local[row][0], local[row][1], local[row][2], local[row][3]);
                }
                ImGui::TreePop();
            }

            // Display LocalToWorld Matrix
            if (ImGui::TreeNode("Local to World Matrix"))
            {
                const glm::mat4& localToWorld = GetLocalToWorldMatrix();
                for (int row = 0; row < 4; ++row)
                {
                    ImGui::Text("%.3f %.3f %.3f %.3f", localToWorld[row][0], localToWorld[row][1], localToWorld[row][2],
                                localToWorld[row][3]);
                }
                ImGui::TreePop();
            }
        }
    }
#endif

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
