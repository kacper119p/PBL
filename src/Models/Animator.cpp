#include "Animator.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp> // Required for glm::to_string()
namespace Models
{
    Animator::Animator(Animation* Animation)
    {
        m_CurrentTime = 0.0;
        m_CurrentAnimation = Animation;

        size_t boneCount = Animation->GetBoneIDMap().size();
        m_FinalBoneMatrices.resize(boneCount, glm::mat4(1.0f));

        m_FinalBoneMatrices.assign(boneCount, glm::mat4(1.0f));

    }
    void Animator::UpdateAnimation(float dt)
    {
        m_DeltaTime = dt;

        if (m_CurrentAnimation && !m_IsPaused)
        {
            float ticksPerSecond = m_CurrentAnimation->GetTicksPerSecond();
            float duration = m_CurrentAnimation->GetDuration();
            float timeAdvance = ticksPerSecond * dt;

            if (m_PlayOnce)
            {
                if (m_IsPlayingBackward)
                {
                    m_CurrentTime -= timeAdvance;
                    if (m_CurrentTime <= m_EndTime)
                    {
                        m_CurrentTime = m_EndTime;
                        m_IsPaused = true;
                        m_PlayOnce = false;
                    }
                }
                else
                {
                    m_CurrentTime += timeAdvance;
                    if (m_CurrentTime >= m_EndTime)
                    {
                        m_CurrentTime = m_EndTime;
                        m_IsPaused = true;
                        m_PlayOnce = false;
                    }
                }
            }
            else
            {
                if (m_IsPlayingBackward)
                {
                    m_CurrentTime -= timeAdvance;
                    if (m_CurrentTime < 0.0f)
                        m_CurrentTime += duration;
                }
                else
                {
                    m_CurrentTime += timeAdvance;
                    m_CurrentTime = fmod(m_CurrentTime, duration);
                }
            }
        }
        CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
    }


    void Animator::PlayAnimation(Animation* pAnimation)
    {
        m_CurrentAnimation = pAnimation;
        m_CurrentTime = 0.0f;

        if (m_CurrentAnimation)
        {
            CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
        }
    }

    void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
    {
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

        if (Bone)
        {
            Bone->Update(m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }


        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int index = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] =m_CurrentAnimation->m_GlobalInverseTransform * globalTransformation * offset;

        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }
    void Animator::PlayOnceFromTo(float startTime, float endTime)
    {
        if (!m_CurrentAnimation)
            return;

        m_StartTime = startTime;
        m_EndTime = endTime;
        m_CurrentTime = startTime;

        m_IsPlayingBackward = startTime > endTime;
        m_PlayOnce = true;
        m_IsPaused = false;

        // Ensure pose is set at start time
        CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
    }

}


