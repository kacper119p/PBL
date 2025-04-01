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
    public:

        Animator(Animation* Animation);
        void UpdateAnimation(float dt);
        void PlayAnimation(Animation* pAnimation);
        void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
        std::vector<glm::mat4> GetFinalBoneMatrices() { return m_FinalBoneMatrices; }
	};
}