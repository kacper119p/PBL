#include "Model.h"

#include "Assimp/Importer.hpp"
#include "Assimp/postprocess.h"
#include "assimp/scene.h"
#include "Vertex.h"
#include "Utility/AssertionsUtility.h"

namespace Models
{
    Model::Model(const char* FilePath)
    {
        Path = std::string(FilePath);
        Assimp::Importer importer = Assimp::Importer();

        const aiScene* scene = importer.ReadFile(FilePath,
                                                 aiProcess_Triangulate | aiProcess_GenNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
            !scene->mRootNode)
        {
            return;
        }

        ProcessNode(scene->mRootNode, scene);
    }

    Model::~Model() = default;

    void Model::ProcessNode(const aiNode* const Node, const aiScene* Scene)
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

    std::unique_ptr<Mesh> Model::ProcessMesh(aiMesh* Mesh)
    {
        std::vector<Vertex> vertices;
        vertices.reserve(Mesh->mNumVertices);
        std::vector<unsigned int> indices;
        indices.reserve(Mesh->mNumFaces);

        CHECK_MESSAGE(Mesh->mVertices != nullptr, "Invalid Mesh format: No position data.")
        CHECK_MESSAGE(Mesh->mTextureCoords != nullptr, "Invalid Mesh format: No UV data.")
        CHECK_MESSAGE(Mesh->mNormals != nullptr, "Invalid Mesh format: No normal Data.")
        CHECK_MESSAGE(Mesh->mTangents != nullptr, "Invalid Mesh format: No tangent data.")

        {
            for (unsigned int i = 0; i < Mesh->mNumVertices; i++)
            {
                glm::vec3 position(Mesh->mVertices[i].x, Mesh->mVertices[i].y, Mesh->mVertices[i].z);
                glm::vec2 texCoord(Mesh->mTextureCoords[0][i].x, Mesh->mTextureCoords[0][i].y);
                glm::vec3 normal(Mesh->mNormals[i].x, Mesh->mNormals[i].y, Mesh->mNormals[i].z);
                glm::vec3 tangent(Mesh->mTangents[i].x, Mesh->mTangents[i].y, Mesh->mTangents[i].z);

                Vertex vertex(position, texCoord, normal, tangent);
                vertices.push_back(vertex);
            }
        }

        for (unsigned int i = 0; i < Mesh->mNumFaces; i++)
        {
            aiFace face = Mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        return std::make_unique<class Mesh>(vertices, indices, std::string(Mesh->mName.C_Str()));
    }
} // Models
