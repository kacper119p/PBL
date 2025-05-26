#include "PrimitiveMeshes.h"
#include <cmath>
#include <glm/gtc/constants.hpp>

namespace Engine
{
    PrimitiveMesh PrimitiveMeshes::BoxMesh;
    PrimitiveMesh PrimitiveMeshes::SphereMesh;
    PrimitiveMesh PrimitiveMeshes::CapsuleMesh;
    PrimitiveMeshes* PrimitiveMeshes::instance_ = nullptr;

    PrimitiveMeshes& PrimitiveMeshes::GetInstance()
    {
        if (!instance_)
            instance_ = new PrimitiveMeshes();
        return *instance_;
    }

    PrimitiveMeshes::PrimitiveMeshes() = default;

    void PrimitiveMeshes::Initialize()
    {
        // Box: unit cube from -1 to 1
        BoxMesh.Vertices = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
                            {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1}};
        BoxMesh.Indices = {
                0, 1, 2, 2, 3, 0, // back
                4, 5, 6, 6, 7, 4, // front
                0, 4, 7, 7, 3, 0, // left
                1, 5, 6, 6, 2, 1, // right
                3, 2, 6, 6, 7, 3, // top
                0, 1, 5, 5, 4, 0 // bottom
        };

        // Sphere generation
        const int latBands = 8;
        const int longBands = 8;
        SphereMesh.Vertices.clear();
        SphereMesh.Indices.clear();

        for (int lat = 0; lat <= latBands; ++lat)
        {
            float theta = lat * glm::pi<float>() / latBands;
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);

            for (int lon = 0; lon <= longBands; ++lon)
            {
                float phi = lon * 2 * glm::pi<float>() / longBands;
                float sinPhi = std::sin(phi);
                float cosPhi = std::cos(phi);

                float x = cosPhi * sinTheta;
                float y = cosTheta;
                float z = sinPhi * sinTheta;
                SphereMesh.Vertices.emplace_back(x, y, z);
            }
        }

        for (int lat = 0; lat < latBands; ++lat)
        {
            for (int lon = 0; lon < longBands; ++lon)
            {
                int first = lat * (longBands + 1) + lon;
                int second = first + longBands + 1;

                SphereMesh.Indices.push_back(first);
                SphereMesh.Indices.push_back(second);
                SphereMesh.Indices.push_back(first + 1);

                SphereMesh.Indices.push_back(second);
                SphereMesh.Indices.push_back(second + 1);
                SphereMesh.Indices.push_back(first + 1);
            }
        }

        // Capsule generation
        CapsuleMesh.Vertices.clear();
        CapsuleMesh.Indices.clear();

        const int segments = 8;
        const int rings = 4;
        const float halfHeight = 1.0f;

        for (int y = -rings; y <= rings; ++y)
        {
            float v = float(y) / rings;
            float height = v * halfHeight;
            for (int i = 0; i <= segments; ++i)
            {
                float angle = i * 2.0f * glm::pi<float>() / segments;
                float x = std::cos(angle);
                float z = std::sin(angle);
                CapsuleMesh.Vertices.emplace_back(x, height, z);
            }
        }

        for (int j = 0; j <= rings; ++j)
        {
            float phi = glm::half_pi<float>() * float(j) / rings;
            float y = std::sin(phi);
            float r = std::cos(phi);
            for (int i = 0; i <= segments; ++i)
            {
                float theta = i * 2.0f * glm::pi<float>() / segments;
                float x = r * std::cos(theta);
                float z = r * std::sin(theta);

                CapsuleMesh.Vertices.emplace_back(x, y + halfHeight, z); // top hemisphere
                CapsuleMesh.Vertices.emplace_back(x, -y - halfHeight, z); // bottom hemisphere
            }
        }
    }

    PrimitiveMesh PrimitiveMeshes::GetBoxMesh(const glm::vec3& position, const glm::quat& rotation, float width,
                                              float height, float depth)
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        model *= glm::toMat4(rotation);

        PrimitiveMesh result;
        result.Vertices = BoxMesh.Vertices;
        result.Indices = BoxMesh.Indices;

        for (auto& v : result.Vertices)
            v *= glm::vec3(width * 0.5f, height * 0.5f, depth * 0.5f);
        for (auto& v : result.Vertices)
            v = glm::vec3(model * glm::vec4(v, 1.0f));
        return result;
    }

    PrimitiveMesh PrimitiveMeshes::GetSphereMesh(const glm::vec3& position, float radius)
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);

        PrimitiveMesh result;
        result.Vertices = SphereMesh.Vertices;
        result.Indices = SphereMesh.Indices;

        for (auto& v : result.Vertices)
            v *= radius;
        for (auto& v : result.Vertices)
            v = glm::vec3(model * glm::vec4(v, 1.0f));
        return result;
    }

    PrimitiveMesh PrimitiveMeshes::GetCapsuleMesh(const glm::vec3& position, const glm::quat& rotation, float radius,
                                                  float height)
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        model *= glm::toMat4(rotation);

        PrimitiveMesh result;
        result.Vertices = CapsuleMesh.Vertices;
        result.Indices = CapsuleMesh.Indices;

        for (auto& v : result.Vertices)
            v *= glm::vec3(radius, height * 0.5f, radius);
        for (auto& v : result.Vertices)
            v = glm::vec3(model * glm::vec4(v, 1.0f));
        return result;
    }
} // namespace Engine
