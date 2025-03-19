#pragma once

#include <vector>
#include <string>

#include "AabBox.h"
#include "Vertex.h"

namespace Models
{
    /**
     * @brief Collection of vertices accessible to the GPU.
     */
    class Mesh final
    {
    public:
        std::vector<Vertex> VerticesData;
        std::vector<unsigned int> VertexIndices;

    private:
        AABBox AABBox;
        unsigned int VertexArray;
        unsigned int VertexBuffer;
        unsigned int ElementBuffer;
        std::string Name;

    public:
        /**
         * @brief Constructs a new mesh using provided data.
         * @param VerticesData Vertices.
         * @param VertexIndices Faces.
         * @param Name Name of the mesh.
         */
        Mesh(const std::vector<Vertex>& VerticesData, const std::vector<unsigned int>& VertexIndices,
             const std::string& Name);

        virtual ~Mesh();

        /**
         * @brief Returns name of this mesh.
         */
        [[nodiscard]] const std::string& GetName() const
        {
            return Name;
        }

        /**
         * @brief Returns VAO id.
         */
        [[nodiscard]] unsigned int GetVertexArray() const
        {
            return VertexArray;
        }

        /**
         * @brief Returns VBO id.
         */
        [[nodiscard]] unsigned int GetVertexBuffer() const
        {
            return VertexBuffer;
        }

        /**
         * @brief Returns EBO id.
         */
        [[nodiscard]] unsigned int GetElementBuffer() const
        {
            return ElementBuffer;
        }

        /**
         * @brief Returns number of vertices in this mesh.
         */
        [[nodiscard]] int GetVertexCount() const
        {
            return static_cast<int>(VerticesData.size());
        }

        /**
         * @brief Returns number of vertex indices in this mesh.
         */
        [[nodiscard]] int GetFaceCount() const
        {
            return static_cast<int>(VertexIndices.size());
        }

        /**
         * @brief Returns AABox for this mesh.
         */
        [[nodiscard]] AABBox3 GetAabBox() const
        {
            return AABBox;
        }

        /**
         * @brief Draws this mesh using currently bound shader.
         */
        void Draw() const;

    private:
        static AABBox3 CreateAABBox(const std::vector<Vertex>& Vertices);
    };
} // Models
