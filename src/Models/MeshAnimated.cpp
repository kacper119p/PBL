#include "Mesh.h"

#include "glad/glad.h"

namespace Models
{
    MeshAnimated::MeshAnimated(const std::vector<Vertex>& VerticesData, const std::vector<unsigned int>& VertexIndices,
               const std::string& Name) :
        VerticesData(VerticesData), VertexIndices(VertexIndices), AABBox(CreateAABBox(VerticesData)), Name(Name)
    {
        glGenVertexArrays(1, &VertexArray);
        glGenBuffers(1, &ElementBuffer);
        glGenBuffers(1, &VertexBuffer);

        glBindVertexArray(VertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, VertexIndices.size() * sizeof(unsigned int), &VertexIndices[0],
                     GL_STATIC_DRAW);

        glBufferData(GL_ARRAY_BUFFER, VerticesData.size() * sizeof(Vertex), &VerticesData[0], GL_STATIC_DRAW);

        // Vertex::Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Position));

        // Vertex::TexCoords
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, TexCoords));

        // Vertex::Normal
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Normal));

        // Vertex::Tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Tangent));

        glBindVertexArray(0);

        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*) offsetof(Vertex, m_BoneIDs));

        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, m_Weights));   
    }

    void MeshAnimated::Draw() const
    {
        glBindVertexArray(VertexArray);
        glDrawElements(GL_TRIANGLES, VertexIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    MeshAnimated::~MeshAnimated()
    {
        glDeleteBuffers(1, &VertexBuffer);
        glDeleteBuffers(1, &ElementBuffer);
        glDeleteVertexArrays(1, &VertexArray);
    }

    AABBox3 MeshAnimated::CreateAABBox(const std::vector<Vertex>& Vertices)
    {
        if (Vertices.empty())
        {
            return AABBox3();
        }
        glm::vec3 min = Vertices[0].Position;
        glm::vec3 max = Vertices[0].Position;
        for (int i = 1; i < Vertices.size(); ++i)
        {
            min = glm::min(min, Vertices[i].Position);
            max = glm::max(max, Vertices[i].Position);
        }
        return AABBox3(min, max);
    }
} // namespace Models
