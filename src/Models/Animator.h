#pragma once

#include <vector>
#include "glad/glad.h"
#include <glm/glm.hpp>
#include "Models/Animation.h"

namespace Models
{
    class Animator
    {
    private:
        std::vector<glm::mat4> m_FinalBoneMatrices;
        Animation* m_CurrentAnimation;
        float m_CurrentTime;
        float m_DeltaTime;
        bool m_IsPaused = false;
        bool m_IsPlayingBackward = false;

        bool m_PlayOnce = false;
        float m_StartTime = 0.0f;
        float m_EndTime = 0.0f;

        float m_AnimationSpeed = 1.0f;

    public:
        Animator() = default;

        Animator(Animation* Animation);

        ~Animator() = default;

        void UpdateAnimation(float dt);

        void PlayAnimation(Animation* pAnimation);

        void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

        void PlayOnceFromTo(float startTime, float endTime);

        std::vector<glm::mat4> GetFinalBoneMatrices() const
        {
            return m_FinalBoneMatrices;
        }

        void PauseAnimation()
        {
            m_IsPaused = true;
        }

        void ResumeAnimation()
        {
            m_IsPaused = false;
        }

        void SetPlayBackward(bool playBackward)
        {
            m_IsPlayingBackward = playBackward;
        }

        bool IsPaused() const
        {
            return m_IsPaused;
        }

        bool IsPlayingBackward() const
        {
            return m_IsPlayingBackward;
        }

        bool IsAnimationFinished() const
        {
            if (m_CurrentAnimation)
            {
                return m_CurrentTime >= m_CurrentAnimation->GetDuration() - 0.5f;
            }
            return true;
        }

        void SetAnimationSpeed(float speed)
        {
            m_AnimationSpeed = speed;
        }

        float GetAnimationSpeed() const
        {
            return m_AnimationSpeed;
        }

    };
}
