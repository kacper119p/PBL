#pragma once

#include <memory>
#include <vector>
#include <map>
#include <string>
#include "MeshAnimated.h"
#include "assimp/scene.h"

#define MAX_BONE_INFLUENCE = 4

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
        std::string Path;

        //for animating
        std::map<std::string, BoneInfo> m_BoneInfoMap; //
        int m_BoneCounter = 0;
        const aiScene* scene;

    public:
        auto& GetBoneInfoMap() { return m_BoneInfoMap; }
        int& GetBoneCount() { return m_BoneCounter; }  
        [[nodiscard]] std::string GetPath() const { return Path; }
        
	static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
        {
            glm::mat4 to;
            // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
            to[0][0] = from.a1;
            to[1][0] = from.a2;
            to[2][0] = from.a3;
            to[3][0] = from.a4;
            to[0][1] = from.b1;
            to[1][1] = from.b2;
            to[2][1] = from.b3;
            to[3][1] = from.b4;
            to[0][2] = from.c1;
            to[1][2] = from.c2;
            to[2][2] = from.c3;
            to[3][2] = from.c4;
            to[0][3] = from.d1;
            to[1][3] = from.d2;
            to[2][3] = from.d3;
            to[3][3] = from.d4;
            return to;
        }

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

        std::unique_ptr<MeshAnimated> ProcessMesh(aiMesh* Mesh);

        void SetVertexBoneDataToDefault(VertexAnimated& vertex);

        void SetVertexBoneData(VertexAnimated& vertex, int boneID, float weight);

        void ExtractBoneWeightForVertices(std::vector<VertexAnimated>& vertices, aiMesh* mesh, const aiScene* scene);
    };
} // namespace Models
