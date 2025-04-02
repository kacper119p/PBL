#include "Model.h"


#include "Assimp/Importer.hpp"
#include "Assimp/postprocess.h"
#include "Vertex.h"
#include "assimp/scene.h"
#include "ModelAnimated.h"
#include "Utility/AssimpGLMHelpers.h"

namespace Models
{
    ModelAnimated::ModelAnimated(const char* FilePath)
    {
        Assimp::Importer importer = Assimp::Importer();

        scene = importer.ReadFile(FilePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            return;
        }

        ProcessNode(scene->mRootNode, scene);
    }

    ModelAnimated::~ModelAnimated() = default;

    void ModelAnimated::ProcessNode(const aiNode* const Node, const aiScene* Scene)
    {
        for (unsigned int i = 0; i < Node->mNumMeshes; ++i)
        {
            aiMesh* mesh = Scene->mMeshes[Node->mMeshes[i]];
            Meshes.push_back(ProcessMesh(mesh));
        }

        for (unsigned int i = 0; i < Node->mNumChildren; ++i)
        {
            ProcessNode(Node->mChildren[i], Scene);
        }
    }

    std::unique_ptr<MeshAnimated> ModelAnimated::ProcessMesh(aiMesh* Mesh)
    {
        std::vector<Vertex> vertices;
        vertices.reserve(Mesh->mNumVertices);
        std::vector<unsigned int> indices;
        indices.reserve(Mesh->mNumFaces);

        for (unsigned int i = 0; i < Mesh->mNumVertices; i++)
        {
            Models::Vertex vertex;
            SetVertexBoneDataToDefault(vertex);
            

            vertex.Position = glm::vec3(Mesh->mVertices[i].x, Mesh->mVertices[i].y, Mesh->mVertices[i].z);
            vertex.Normal = glm::vec3(Mesh->mNormals[i].x, Mesh->mNormals[i].y, Mesh->mNormals[i].z);

            if (Mesh->mTextureCoords[0]) // Check if there are texture coordinates
                vertex.TexCoords = glm::vec2(Mesh->mTextureCoords[0][i].x, Mesh->mTextureCoords[0][i].y);
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertex.Tangent = Mesh->mTangents
                                     ? glm::vec3(Mesh->mTangents[i].x, Mesh->mTangents[i].y, Mesh->mTangents[i].z)
                                     : glm::vec3(0.0f, 0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        // Process indices
        for (unsigned int i = 0; i < Mesh->mNumFaces; i++)
        {
            aiFace face = Mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // Extract bone weight influences
        ExtractBoneWeightForVertices(vertices, Mesh, scene);

        return std::make_unique<class MeshAnimated>(vertices, indices, std::string(Mesh->mName.C_Str()));
    }

    void ModelAnimated::SetVertexBoneDataToDefault(Vertex& vertex)
    {
        for (int i = 0; i < 4; i++)
        {
            vertex.m_BoneIDs[i] = -1;
            vertex.m_Weights[i] = 0.0f;
        }
    }

    void ModelAnimated::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (vertex.m_BoneIDs[i] < 0)
            {
                vertex.m_Weights[i] = weight;
                vertex.m_BoneIDs[i] = boneID;
                break;
            }
        }
    }



    void ModelAnimated::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
    {
        auto& boneInfoMap = m_BoneInfoMap;
        int& boneCount = m_BoneCounter;
        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id = boneCount;
                newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
                boneInfoMap[boneName] = newBoneInfo;
                boneID = boneCount;
                boneCount++;
            }
            else
            {
                boneID = boneInfoMap[boneName].id;
            }
            assert(boneID != -1);
            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= vertices.size());
                SetVertexBoneData(vertices[vertexId], boneID, weight);
            }
        }
    }




}
