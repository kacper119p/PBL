#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

namespace Engine
{

    struct PrimitiveMesh
    {
        std::vector<glm::vec3> Vertices;
        std::vector<unsigned int> Indices;
    };

    class PrimitiveMeshes
    {
    public:
        static PrimitiveMeshes& GetInstance();

        static void Initialize();

        PrimitiveMesh GetBoxMesh(const glm::vec3& position, const glm::quat& rotation, float width, float height,
                                 float depth);
        PrimitiveMesh GetSphereMesh(const glm::vec3& position, float radius);
        PrimitiveMesh GetCapsuleMesh(const glm::vec3& position, const glm::quat& rotation, float radius, float height);

    private:
        PrimitiveMeshes();
        PrimitiveMeshes(const PrimitiveMeshes&) = delete;
        PrimitiveMeshes& operator=(const PrimitiveMeshes&) = delete;

        static PrimitiveMesh BoxMesh;
        static PrimitiveMesh SphereMesh;
        static PrimitiveMesh CapsuleMesh;

        static PrimitiveMeshes* instance_;
    };
} // namespace Engine
