#pragma once

#include <memory>
#include <vector>
#include <map>
#include <string>
#include "MeshAnimated.h"
#include "assimp/scene.h"

#define MAX_BONE_WEIGHTS = 4

namespace Models
{
    /**
     * @brief Collection of meshes and bones
     * 
     */

    struct BoneInfo
    {
        /*id is index in finalBoneMatrices*/
        int id;

        /*offset matrix transforms vertex from model space to bone space*/
        glm::mat4 offset;
    };

    class ModelAnimated
    {
    private:
        std::vector<std::unique_ptr<MeshAnimated>> Meshes = std::vector<std::unique_ptr<MeshAnimated>>();

        //for animating
        std::map<std::string, BoneInfo> m_BoneInfoMap; //
        int m_BoneCounter = 0;

        auto& GetBoneInfoMap() { return m_BoneInfoMap; }
        int& GetBoneCount() { return m_BoneCounter; }  

    public:
        /**
         * @brief Constructs a new model from a given file.
         * @param FilePath Path to a model file.
         */
        explicit ModelAnimated(const char* FilePath);

    public:
        virtual ~ModelAnimated();

    public:
        /**
         * @brief Returns number of meshes contained by this model.
         */
        [[nodiscard]] unsigned int GetMeshCount() const { return Meshes.size(); }

        /**
         * @brief Finds mesh by index.
         * @param Index Index of a mesh.
         * @return Mesh.
         */
        [[nodiscard]] MeshAnimated* GetMesh(const int Index) const { return Meshes[Index].get(); }

        /**
         * @brief Finds mesh by name.
         * @param Name Name of a mesh.
         * @return Mesh.
         */
        [[nodiscard]] MeshAnimated* GetMesh(const char* const Name) const
        {
            for (int i = 0; i < Meshes.size(); ++i)
            {
                if (Meshes[i]->GetName() == Name)
                {
                    return Meshes[i].get();
                }
            }
            return nullptr;
        }

    private:
        void ProcessNode(const aiNode* Node, const aiScene* Scene);

        static std::unique_ptr<MeshAnimated> ProcessMesh(aiMesh* Mesh);

        void SetVertexBoneDataToDefault(Vertex& vertex);

        void SetVertexBoneData(Vertex& vertex, int boneID, float weight);

        void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
    };
} // namespace Models
