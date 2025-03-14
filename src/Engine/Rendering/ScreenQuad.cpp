#include "ScreenQuad.h"
#include "glad/glad.h"

Engine::Rendering::ScreenQuad::ScreenQuad()
{
    constexpr float vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top right
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    };

    constexpr unsigned int faceIndices[] = {
            3, 2, 1,
            0, 3, 1
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (0 * sizeof(float)));

    //TexCoords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));

    glBindVertexArray(0);
}

Engine::Rendering::ScreenQuad::~ScreenQuad()
{
    glDeleteBuffers(1, &VertexBuffer);
    glDeleteBuffers(1, &ElementBuffer);
    glDeleteVertexArrays(1, &VertexArray);
}

void Engine::Rendering::ScreenQuad::Draw()
{
    glBindVertexArray(VertexArray);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
