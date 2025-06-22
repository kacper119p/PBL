#include "ScreenQuad.h"
#include "glad/glad.h"

struct Engine::Rendering::ScreenQuad::CachedData Engine::Rendering::ScreenQuad::CachedData;

Engine::Rendering::ScreenQuad::CachedData::~CachedData()
{
    glDeleteBuffers(1, &VertexBuffer);
    glDeleteVertexArrays(1, &VertexArray);
}

Engine::Rendering::ScreenQuad::~ScreenQuad() = default;

void Engine::Rendering::ScreenQuad::Draw()
{
    if (CachedData.VertexBuffer == 0)
    {
        Initialize();
    }
    glBindVertexArray(CachedData.VertexArray);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void Engine::Rendering::ScreenQuad::Initialize()
{
    constexpr float vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
            3.0f, -1.0f, 0.0f, 2.0f, 0.0f, // bottom right way off-screen
            -1.0f, 3.0f, 0.0f, 0.0f, 2.0f // top left way off-screen
    };

    glGenVertexArrays(1, &CachedData.VertexArray);
    glGenBuffers(1, &CachedData.VertexBuffer);

    glBindVertexArray(CachedData.VertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, CachedData.VertexBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
                 vertices, GL_STATIC_DRAW);

    //Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0 * sizeof(float)));

    //TexCoords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

    glBindVertexArray(0);
}
