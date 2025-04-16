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
        if (m_CurrentAnimation)
        {
            m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
            m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
            CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
        }
    }
    void Animator::PlayAnimation(Animation* pAnimation)
    {
        m_CurrentAnimation = pAnimation;
        m_CurrentTime = 0.0f;
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
            m_FinalBoneMatrices[index] = offset * globalTransformation;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }
}


