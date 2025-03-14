#pragma once

#include <vector>
#include "glm/glm.hpp"

namespace Engine
{
    class Entity;

    /**
     * @brief Position, rotation and scale of an entity.
     */
    class Transform
    {
    private:
        glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 EulerAngles = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f);

        glm::mat4 LocalMatrix = glm::mat4(1.0f);
        glm::mat4 LocalToWorldMatrix = glm::mat4(1.0f);;
        bool IsDirty = false;

    private:
        Transform* Parent = nullptr;
        Entity* Owner;
        std::vector<Transform*> Children = std::vector<Transform*>();

    public:
        /**
         * @brief Creates a new transform representing an identity transformation.
         * @param Owner Owner of this transform.
         */
        explicit Transform(Entity* Owner) :
            Owner(Owner)
        {
        }

    public:
        virtual ~Transform();

    public:
        /**
         * @brief Returns position in local space.
         */
        [[nodiscard]] const glm::vec3& GetPosition() const
        {
            return Position;
        }

        /**
         * @brief Returns position in world space.
         */
        [[nodiscard]] glm::vec3 GetPositionWorldSpace()
        {
            return GetLocalToWorldMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        }

        /**
         * @brief Sets position in local space.
         * @param InPosition New position.
         */
        void SetPosition(const glm::vec3& InPosition)
        {
            Position = InPosition;
            MarkDirty();
        }

        /**
         * @brief Sets position in world space.
         * @param InPosition New position.
         */
        void SetPositionWorldSpace(const glm::vec3& InPosition)
        {
            Position = glm::vec3(glm::inverse(GetLocalToWorldMatrix()) * glm::vec4(InPosition, 1.0f));
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
         * @param InEulerAngles New rotation.
         */
        void SetEulerAngles(const glm::vec3& InEulerAngles)
        {
            EulerAngles = InEulerAngles;
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
         * @brief Returns Entity owning this transform
         */
        [[nodiscard]] Entity* GetOwner() const
        {
            return Owner;
        }

        /**
         * @brief Returns local to parent's space transformation matrix.
         */
        const glm::mat4& GetLocalMatrix();

        /**
         * @brief Returns local to world space transformation matrix.
         */
        const glm::mat4& GetLocalToWorldMatrix();

        /**
         * @brief Sets a new parent of this entity. Removes previous one if it has one.
         * @param InParent Parent.
         */
        void SetParent(Transform* InParent);

        /**
         * @brief Returns parent of this transform.
         */
        [[nodiscard]] Transform* GetParent() const;

        /**
         * @brief Attaches a new child to this transform.
         * @param Child A new child.
         */
        void AddChild(Transform* Child);

        /**
         * @brief Removes a child from this transform.
         * @param Child Child to be removed.
         */
        void RemoveChild(Transform* Child);

        /**
         * @brief Returns all children of this transform.
         */
        [[nodiscard]] const std::vector<Transform*>& GetChildren();

    private:
        void MarkDirty();

        void UpdateMatrices();
    };
}
