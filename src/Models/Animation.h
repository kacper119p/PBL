#pragma once

#include <glm/glm.hpp>
#include "Bone.h"
#include "ModelAnimated.h"
#include <map>
#include <assimp/scene.h>
namespace Models
{
    struct AssimpNodeData
    {
        glm::mat4 transformation;
        std::string name;
        int childrenCount;
        std::vector<AssimpNodeData> children;
    };

    class Animation
    {
    private:
        float m_Duration;
        int m_TicksPerSecond;
        std::vector<Bone> m_Bones;
        AssimpNodeData m_RootNode;
        std::map<std::string, BoneInfo> m_BoneInfoMap;
        std::string Path;

    public:
        Animation() = default;
        Animation(const std::string& animationPath, ModelAnimated* model);

        ~Animation() {}

        Bone* FindBone(const std::string& name);

        inline float GetTicksPerSecond() { return m_TicksPerSecond; }

        inline float GetDuration() { return m_Duration; }

        inline const AssimpNodeData& GetRootNode() { return m_RootNode; }

        inline const std::map<std::string, BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }

        [[nodiscard]] std::string GetPath() const { return Path; }

    private:
        void ReadMissingBones(const aiAnimation* animation, ModelAnimated& model);
        void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);



    };
}