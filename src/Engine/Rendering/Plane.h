#pragma once
#include <cstdint>
#include <glm/vec3.hpp>

namespace Engine
{

    class Plane
    {
    private:
        static constexpr uint32_t VertexCount = 4;
        static constexpr uint32_t FaceCount = 6;
        static constexpr uint32_t VertexSize = 5;

        struct CachedData
        {
            unsigned int VertexArray = 0;
            unsigned int VertexBuffer = 0;
            unsigned int ElementBuffer = 0;

            ~CachedData();
        };

        static constexpr float Vertices[VertexCount * VertexSize] = {
                -0.5f, 0.0f, 0.5f, 0.0f, 0.0f, // bottom left
                -0.5f, 0.0f, -0.5f, 0.0f, 1.0f, // top left
                0.5f, 0.0f, -0.5f, 1.0f, 1.0f, // top right
                0.5f, 0.0f, 0.5f, 1.0f, 0.0f, // bottom right
        };

        static constexpr uint32_t FaceIndices[FaceCount] = {
                3, 2, 1,
                0, 3, 1
        };

    public:
        [[nodiscard]] static const float* GetVertices()
        {
            return Vertices;
        }

        [[nodiscard]] static const uint32_t* GetIndices()
        {
            return FaceIndices;
        }

        [[nodiscard]] static uint32_t GetVertexSize()
        {
            return VertexSize;
        }

        [[nodiscard]] static uint32_t GetFaceCount()
        {
            return FaceCount;
        }

        [[nodiscard]] static glm::vec3 GetVertexPosition(const size_t VertexIndex)
        {
            return glm::vec3(Vertices[VertexIndex * VertexSize],
                             Vertices[VertexIndex * VertexSize + 1],
                             Vertices[VertexIndex * VertexSize + 2]);
        }

    private:
        static CachedData CachedData;

    private:
        Plane();

    private:
        ~Plane();

    public:
        /**
         * @brief Draws quad covering whole screen
         */
        static void Draw();

    private:
        static void Initialize();
    };

} // Engine
