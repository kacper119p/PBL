#include "CubeGeometry.h"

#include "glad/glad.h"

namespace Engine
{
    Engine::CubeGeometry::CubeGeometry()
    {
        constexpr float vertices[] = {
                -0.5f, -0.5f, -0.5f, //0
                0.5f, -0.5f, -0.5f, //1
                -0.5f, 0.5f, -0.5f, //2
                0.5f, 0.5f, -0.5f, //3
                -0.5f, -0.5f, 0.5f, //4
                0.5f, -0.5f, 0.5f, //5
                -0.5f, 0.5f, 0.5f, //6
                0.5f, 0.5f, 0.5f, //7
        };

        constexpr unsigned int faceIndices[] = {
                0, 1, 3,
                3, 2, 0,
                7, 5, 4,
                4, 6, 7,
                0, 2, 6,
                6, 4, 0,
                3, 1, 7,
                5, 7, 1,
                5, 1, 0,
                0, 4, 5,
                3, 7, 2,
                6, 2, 7,
        };

        glGenVertexArrays(1, &VertexArray);
        glGenBuffers(1, &ElementBuffer);
        glGenBuffers(1, &VertexBuffer);

        glBindVertexArray(VertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faceIndices), faceIndices,
                     GL_STATIC_DRAW);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
                     vertices, GL_STATIC_DRAW);

        //Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) (0 * sizeof(float)));
        glBindVertexArray(0);
    }

    Engine::CubeGeometry::~CubeGeometry()
    {
        glDeleteBuffers(1, &VertexBuffer);
        glDeleteBuffers(1, &ElementBuffer);
        glDeleteVertexArrays(1, &VertexArray);
    }

    void Engine::CubeGeometry::Draw()
    {
        glBindVertexArray(VertexArray);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

} // Engine
