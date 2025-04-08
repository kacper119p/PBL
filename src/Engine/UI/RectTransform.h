#pragma once
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace Engine::Ui
{
    class UiElement;
}

namespace Engine::Ui
{

    class RectTransform final
    {
    private:
        glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
        float Rotation = 0.0f;
        glm::vec2 Size = glm::vec2(1.0f, 1.0f);

        glm::mat4 LocalMatrix = glm::mat4(1.0f);
        glm::mat4 LocalToWorldMatrix = glm::mat4(1.0f);
        glm::mat4 LocalToWorldMatrixNoScale = glm::mat4(1.0f);
        bool IsDirty = true;

    private:
        RectTransform* Parent = nullptr;
        UiElement* Owner = nullptr;
        std::vector<RectTransform*> Children = std::vector<RectTransform*>();

    public:
        /**
         * @brief Initializes Transform with default values.
         */
        RectTransform() = default;

        /**
         * @brief Creates a new transform representing an identity transformation.
         * @param Owner Owner of this transform.
         */
        explicit RectTransform(UiElement* Owner) :
            Owner(Owner)
        {
        }

    public:
        virtual ~RectTransform();

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
         * @brief Sets position in local space in pixels. Assumes 1920x1080 screen.
         * @param InPosition New position.
         */
        void SetPositionPixels(const glm::vec3& InPosition)
        {
            Position.x = InPosition.x / 1920.0f;
            Position.y = InPosition.y / 1080.0f;
            Position.z = InPosition.z;
            MarkDirty();
        }

        /**
         * @brief Sets size of this rect in pixels. Assumes 1920x1080 screen.
         * @param Value A new position.
         */
        void SetSizePixels(const glm::vec2& Value)
        {
            Position.x = Value.x / 1920.0f;
            Position.y = Value.y / 1080.0f;
            MarkDirty();
        }

        /**
         * @brief Returns size of this rect in pixels. Assumes 1920x1080 screen.
         */
        [[nodiscard]] glm::vec2 GetSizePixels()
        {
            return glm::vec2(Position.x * 1920.0f,
                             Position.y * 1080.0f);
            MarkDirty();
        }

        /**
         * @brief Sets position in world space.
         * @param InPosition New position.
         */
        void SetPositionWorldSpace(const glm::vec3& InPosition)
        {
            Position = glm::vec3(glm::inverse(GetLocalToWorldMatrix()) * glm::vec4(InPosition, 1.0f));
            MarkDirty();
        }

        /**
         * @brief Returns rotation of this transform represented in degrees.
         */
        [[nodiscard]] const float GetRotation() const
        {
            return Rotation;
        }

        /**
         * @brief Sets rotation of this transform represented in degrees.
         * @param Value New rotation.
         */
        void SetRotation(const float Value)
        {
            Rotation = Value;
            MarkDirty();
        }

        /**
         * @brief Returns local scale represented by this transform.
         */
        [[nodiscard]] const glm::vec2& GetSize() const
        {
            return Size;
        }

        /**
         * @brief Sets a new scale in local space for this transform.
         * @param InScale New scale.
         */
        void SetScale(const glm::vec2& InScale)
        {
            Size = InScale;
            MarkDirty();
        }

        /**
         * @brief Returns Entity owning this transform
         */
        [[nodiscard]] UiElement* GetOwner() const
        {
            return Owner;
        }

        /**
         * @brief Attaches this transform to a parent.
         * @param InParent A new parent.
         */
        void SetParent(RectTransform* InParent);

        /**
         * @brief Returns local to parent's space transformation matrix.
         */
        const glm::mat4& GetLocalMatrix();

        /**
         * @brief Returns local to world space transformation matrix.
         */
        const glm::mat4& GetLocalToWorldMatrix();

        /**
         * @brief Returns local to world space transformation matrix without scale component.
         */
        const glm::mat4& GetLocalToWorldMatrixNoScale();

        /**
         * @brief Returns parent of this transform.
         */
        [[nodiscard]] RectTransform* GetParent() const
        {
            return Parent;
        }

        /**
         * @brief Attaches a new child to this transform.
         * @param Child A new child.
         */
        void AddChild(RectTransform* Child);

        /**
         * @brief Removes a child from this transform.
         * @param Child Child to be removed.
         */
        void RemoveChild(RectTransform* Child);

        /**
         * @brief Returns all children of this transform.
         */
        [[nodiscard]] const std::vector<RectTransform*>& GetChildren();

    private:
        void SetOwner(UiElement* const InOwner)
        {
            Owner = InOwner;
        }

    private:
        void MarkDirty();

        void UpdateMatrices();

    public:
        [[nodiscard]] std::vector<RectTransform*>::iterator begin()
        {
            return Children.begin();
        }

        [[nodiscard]] std::vector<RectTransform*>::iterator end()
        {
            return Children.end();
        }

        [[nodiscard]] std::vector<RectTransform*>::const_iterator begin() const
        {
            return Children.begin();
        }

        [[nodiscard]] std::vector<RectTransform*>::const_iterator end() const
        {
            return Children.end();
        }
    };

}
