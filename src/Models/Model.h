#pragma once

#include <memory>
#include <vector>
#include "Mesh.h"
#include "assimp/scene.h"

namespace Models
{
    /**
     * @brief Collection of meshes.
     */
    class Model
    {
    private:
        std::vector<std::unique_ptr<Mesh> > Meshes = std::vector<std::unique_ptr<Mesh> >();

    public:
        /**
         * @brief Constructs a new model from a given file.
         * @param FilePath Path to a model file.
         */
        explicit Model(const char* FilePath);

    public:
        virtual ~Model();

    public:
        /**
         * @brief Returns number of meshes contained by this model.
         */
        [[nodiscard]] unsigned int GetMeshCount() const
        {
            return Meshes.size();
        }

        /**
         * @brief Finds mesh by index.
         * @param Index Index of a mesh.
         * @return Mesh.
         */
        [[nodiscard]] Mesh* GetMesh(const int Index) const
        {
            return Meshes[Index].get();
        }

        /**
         * @brief Finds mesh by name.
         * @param Name Name of a mesh.
         * @return Mesh.
         */
        [[nodiscard]] Mesh* GetMesh(const char* const Name) const
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

        static std::unique_ptr<Mesh> ProcessMesh(aiMesh* Mesh);
    };
} // Models
