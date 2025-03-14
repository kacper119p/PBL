#pragma once

#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace Engine
{
    class Camera
    {
    private:
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 forward;
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 transform = glm::lookAt(position, position + forward, up);
        glm::mat4 projectionMatrix;
        glm::float32 yaw = -glm::pi<float>() / 2.0f;
        glm::float32 pitch = 0.0f;
        glm::float32 sensitivity;

    private:
        bool isDragged = false;

    public:
        explicit Camera(const glm::mat4& projectionMatrix, glm::float32 sensitivity);

    public:
        [[nodiscard]] const glm::vec3& GetPosition() const
        {
            return position;
        }

        void SetPosition(const glm::vec3& value)
        {
            position = value;
            UpdateTransform();
        }

        [[nodiscard]] const glm::vec3& GetForward() const
        {
            return forward;
        }

        [[nodiscard]] const glm::vec3& GetUp() const
        {
            return up;
        }

        [[nodiscard]] const glm::mat4& GetTransform() const
        {
            return transform;
        }

        [[nodiscard]] const glm::mat4& GetProjectionMatrix() const
        {
            return projectionMatrix;
        }

        void SetProjectionMatrix(const glm::mat4& value)
        {
            projectionMatrix = value;
        }

        [[nodiscard]] glm::float32 GetYaw() const
        {
            return yaw;
        }

        void SetYaw(glm::float32 value)
        {
            yaw = value;
            forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            forward = glm::normalize(forward);
            UpdateTransform();
        }

        [[nodiscard]] glm::float32 GetPitch() const
        {
            return pitch;
        }

        void SetPitch(glm::float32 value)
        {
            this->pitch = value;
            forward.x = cos(yaw) * cos(pitch);
            forward.y = sin(pitch);
            forward.z = sin(yaw) * cos(pitch);
            forward = glm::normalize(forward);
            UpdateTransform();
        }

        void SetRotation(glm::float32 pitch, glm::float32 yaw)
        {
            this->pitch = pitch;
            this->yaw = yaw;
            forward.x = cos(yaw) * cos(pitch);
            forward.y = sin(pitch);
            forward.z = sin(yaw) * cos(pitch);
            forward = glm::normalize(forward);
            UpdateTransform();
        }

        void SetPositionAndRotation(glm::vec3 position, glm::float32 pitch, glm::float32 yaw)
        {
            this->position = position;
            this->pitch = pitch;
            this->yaw = yaw;
            forward.x = cos(yaw) * cos(pitch);
            forward.y = sin(pitch);
            forward.z = sin(yaw) * cos(pitch);
            forward = glm::normalize(forward);
            UpdateTransform();
        }

        [[nodiscard]] glm::float32 GetSensitivity() const
        {
            return sensitivity;
        }

        void SetSensitivity(glm::float32 value)
        {
            this->sensitivity = value;
        }

        [[nodiscard]] bool IsDragged() const
        {
            return isDragged;
        }

        void SetIsDragged(bool value)
        {
            this->isDragged = value;
        }

    private:
        void UpdateTransform()
        {
            transform = glm::lookAt(position, position + forward, up);
        }
    };
} // Engine
