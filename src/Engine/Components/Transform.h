#pragma once
#include <vector>

#include "Serialization/SerializedObject.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/euler_angles.hpp"

#if EDITOR
#include "imgui.h"
#endif

namespace Engine
{
    class Entity;

    /**
     * @brief Position, rotation and scale of an entity.
     */
    class Transform final : public Serialization::SerializedObject
    {
        friend class Entity;

    private:
        glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 EulerAngles = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::quat Rotation = glm::quat();

        glm::mat4 LocalMatrix = glm::mat4(1.0f);
        glm::mat4 LocalToWorldMatrix = glm::mat4(1.0f);
        bool IsDirty = true;

    private:
        Transform* Parent = nullptr;
        Entity* Owner = nullptr;
        std::vector<Transform*> Children = std::vector<Transform*>();

    public:
        /**
         * @brief Initializes Transform with default values.
         */
        Transform() = default;

        /**
         * @brief Creates a new transform representing an identity transformation.
         * @param Owner Owner of this transform.
         */
        explicit Transform(Entity* Owner) :
            Owner(Owner)
        {
        }

    public:
        ~Transform() override;

    public:
        /**
         * @brief Returns position in world space.
         */
        [[nodiscard]] glm::vec3 GetPositionWorldSpace()
        {
            return GetLocalToWorldMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        }

        /**
        * @brief Returns position in world space.
        */
        [[nodiscard]] glm::vec3 GetPosition()
        {
            return GetPositionWorldSpace();
        }

        /**
        * @brief Returns position in parent's space.
        */
        [[nodiscard]] const glm::vec3& GetPositionLocalSpace() const
        {
            return Position;
        }

        /**
         * @brief Sets position in world space.
         * @param InPosition New position.
         */
        void SetPosition(const glm::vec3& InPosition)
        {
            if (Parent != nullptr)
            {
                Position = glm::vec3(glm::inverse(Parent->GetLocalToWorldMatrix()) * glm::vec4(InPosition, 1.0f));
            }
            else
            {
                Position = InPosition;
            }
            MarkDirty();
        }

        /**
         * @brief Sets position in world space.
         * @param InPosition New position.
         */
        void SetPositionWorldSpace(const glm::vec3& InPosition)
        {
            if (Parent != nullptr)
            {
                Position = glm::vec3(glm::inverse(Parent->GetLocalToWorldMatrix()) * glm::vec4(InPosition, 1.0f));
            }
            else
            {
                Position = InPosition;
            }
            MarkDirty();
        }

        /**
         * @brief Sets position in parent's space.
         * @param InPosition New position.
         */
        void SetPositionLocalSpace(const glm::vec3& InPosition)
        {
            Position = InPosition;
            MarkDirty();
        }

        /**
         * @brief Returns rotation of this transform represented in euler angles in degrees.
         */
        [[nodiscard]] const glm::vec3& GetEulerAngles() const
        {
            return EulerAngles;
        }

        /**
         * @brief Sets rotation of this transform represented in euler angles in degrees.
         * @param EulerAngles New rotation.
         */
        void SetEulerAngles(const glm::vec3& EulerAngles)
        {
            this->EulerAngles = EulerAngles;
            Rotation = glm::quat(glm::radians(EulerAngles));
            MarkDirty();
        }

        /**
         * @brief Sets rotation of this transform represented by quaternion.
         * @return
         */
        [[nodiscard]] glm::quat GetRotation() const
        {
            return Rotation;
        }

        /**
         * @brief Returns rotation of this transform represented iby quaternion.
         */
        void SetRotation(const glm::quat& Rotation)
        {
            this->Rotation = Rotation;
            EulerAngles = glm::degrees(glm::eulerAngles(Rotation));
            MarkDirty();
        }

        /**
         * @brief Returns local scale represented by this transform.
         */
        [[nodiscard]] const glm::vec3& GetScale() const
        {
            return Scale;
        }

        /**
         * @brief Sets a new scale in local space for this transform.
         * @param InScale New scale.
         */
        void SetScale(const glm::vec3& InScale)
        {
            Scale = InScale;
            MarkDirty();
        }

        /**
         * @brief Transforms position from world space to local space.
         * @param Position Position in world space.
         * @return Position in local space.
         */
        [[nodiscard]] glm::vec3 TransformPositionWorldToLocal(const glm::vec3& Position)
        {
            return glm::vec3(glm::inverse(GetLocalToWorldMatrix()) * glm::vec4(Position, 1.0f));
        }

        /**
         * @brief Transforms position from local space to world space.
         * @param Position Position in local space.
         * @return Position in world space.
         */
        [[nodiscard]] glm::vec3 TransformPositionLocalToWorld(const glm::vec3& Position)
        {
            return glm::vec3(GetLocalToWorldMatrix() * glm::vec4(Position, 1.0f));
        }

        /**
         * @brief Transforms direction from world space to local space.
         * @param Direction Direction in world space.
         * @return Direction in local space.
         */
        [[nodiscard]] glm::vec3 TransformDirectionWorldToLocal(const glm::vec3& Direction)
        {
            return glm::normalize(glm::vec3(glm::inverse(GetLocalToWorldMatrix()) * glm::vec4(Direction, 0.0f)));
        }

        /**
         * @brief Transforms direction from local space to world space.
         * @param Direction Direction in local space.
         * @return Direction in world space.
         */
        [[nodiscard]] glm::vec3 TransformDirectionLocalToWorld(const glm::vec3& Direction)
        {
            return glm::normalize(glm::vec3(GetLocalToWorldMatrix() * glm::vec4(Direction, 0.0f)));
        }

        /**
         * @brief Returns Entity owning this transform
         */
        [[nodiscard]] Entity* GetOwner() const
        {
            return Owner;
        }

        /**
         * @brief Returns local to parent's space transformation matrix.
         */
        const glm::mat4& GetLocalMatrix()
        {
            UpdateMatrices();
            return LocalMatrix;
        }

        /**
         * @brief Returns local to world space transformation matrix.
         */
        const glm::mat4& GetLocalToWorldMatrix()
        {
            UpdateMatrices();
            return LocalToWorldMatrix;
        }

        /**
         * @brief Returns right orientation vector of this transform.
         */
        [[nodiscard]] glm::vec3 GetRight()
        {
            return glm::normalize(glm::vec3(GetLocalToWorldMatrix()[0]));
        }

        /**
         * @brief Returns up orientation vector of this transform.
         */
        [[nodiscard]] glm::vec3 GetUp()
        {
            return glm::normalize(glm::vec3(GetLocalToWorldMatrix()[1]));
        }

        /**
         *@brief Returns forward orientation vector of this transform.
         */
        [[nodiscard]] glm::vec3 GetForward()
        {
            return glm::normalize(-glm::vec3(GetLocalToWorldMatrix()[2]));
        }

        /**
         * @brief Returns left orientation vector of this transform.
         */
        [[nodiscard]] glm::vec3 GetLeft()
        {
            return -GetRight();
        }

        /**
         * @brief Returns down orientation vector of this transform.
         */
        [[nodiscard]] glm::vec3 GetDown()
        {
            return -GetUp();
        }

        /**
         *@brief Returns backwards orientation vector of this transform.
         */
        [[nodiscard]] glm::vec3 GetBackwards()
        {
            return -GetForward();
        }


        /**
         * @brief Sets a new parent of this entity. Removes previous one if it has one.
         * @param InParent Parent.
         */
        void SetParent(Transform* InParent)
        {
            if (Parent != nullptr)
            {
                Parent->RemoveChild(this);
            }
            Parent = InParent;
            Parent->AddChild(this);
            MarkDirty();
        }

        /**
         * @brief Returns parent of this transform.
         */
        [[nodiscard]] Transform* GetParent() const
        {
            return Parent;
        }

        /**
         * @brief Attaches a new child to this transform.
         * @param Child A new child.
         */
        void AddChild(Transform* Child)
        {
            if (Child->Parent != nullptr)
            {
                Child->Parent->RemoveChild(Child);
            }
            Child->Parent = this;
            Child->MarkDirty();
            Children.push_back(Child);
        }

        /**
         * @brief Removes a child from this transform.
         * @param Child Child to be removed.
         */
        void RemoveChild(Transform* Child)
        {
            if (std::erase(Children, Child))
            {
                Child->Parent = nullptr;
                Child->MarkDirty();
            }
        }

        /**
         * @brief Returns all children of this transform.
         */
        [[nodiscard]] const std::vector<Transform*>& GetChildren()
        {
            return Children;
        }

    private:
        void SetOwner(Entity* const InOwner)
        {
            Owner = InOwner;
        }

    private:
        void MarkDirty();

        void UpdateMatrices();

    public:
        [[nodiscard]] std::vector<Transform*>::iterator begin()
        {
            return Children.begin();
        }

        [[nodiscard]] std::vector<Transform*>::iterator end()
        {
            return Children.end();
        }

        [[nodiscard]] std::vector<Transform*>::const_iterator begin() const
        {
            return Children.begin();
        }

        [[nodiscard]] std::vector<Transform*>::const_iterator end() const
        {
            return Children.end();
        }

#if EDITOR
        void DrawImGui();
#endif

        SERIALIZATION_EXPORT_CLASS(Transform)
    };
}
