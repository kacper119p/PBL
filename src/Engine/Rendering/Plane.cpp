#include "Plane.h"
#include "glad/glad.h"

struct Engine::Plane::CachedData Engine::Plane::Plane::CachedData;

Engine::Plane::Plane::CachedData::~CachedData()
{
    glDeleteBuffers(1, &VertexBuffer);
    glDeleteBuffers(1, &ElementBuffer);
    glDeleteVertexArrays(1, &VertexArray);
}

Engine::Plane::Plane()
{
    Initialize();
}

Engine::Plane::~Plane() = default;

void Engine::Plane::Draw()
{
    static Plane plane;
    glBindVertexArray(CachedData.VertexArray);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Engine::Plane::Initialize()
{
    glGenVertexArrays(1, &CachedData.VertexArray);
    glGenBuffers(1, &CachedData.ElementBuffer);
    glGenBuffers(1, &CachedData.VertexBuffer);

    glBindVertexArray(CachedData.VertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, CachedData.VertexBuffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CachedData.ElementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(FaceIndices), FaceIndices,
                 GL_STATIC_DRAW);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices),
                 Vertices, GL_STATIC_DRAW);

    //Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0 * sizeof(float)));

    //TexCoords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

    glBindVertexArray(0);
}
