
#include "NavMesh.h"
#include "Engine/Components/Renderers/ModelRenderer.h"
#include "spdlog/spdlog.h"

namespace Engine
{
    NavMesh& NavMesh::Get()
    {
        static NavMesh instance;
        return instance;
    }

    Graph* NavMesh::GetGraph()
    {
        return NavGraph.get();
    }

    void NavMesh::RemoveNotWalkableNodes(Entity* Root)
    {
        std::vector<Transform*> entities = Root->GetTransform()->GetChildren();
        for (const auto& entity : entities)
        {
            auto* navArea = entity->GetOwner()->GetComponent<NavArea>();
            if (!navArea || navArea->GetWalkable())
                continue;

            auto* modelRenderer = entity->GetOwner()->GetComponent<ModelRenderer>();
            if (!modelRenderer)
                continue;

            Models::Model* model = modelRenderer->GetModel();
            glm::mat4 localToWorld = entity->GetLocalToWorldMatrix();

            for (int i = 0; i < model->GetMeshCount(); ++i)
            {
                const Models::AABBox3& bounds = model->GetMesh(i)->GetAabBox();

                glm::vec3 corners[8] = {
                        bounds.min,
                        glm::vec3(bounds.min.x, bounds.min.y, bounds.max.z),
                        glm::vec3(bounds.min.x, bounds.max.y, bounds.min.z),
                        glm::vec3(bounds.min.x, bounds.max.y, bounds.max.z),
                        glm::vec3(bounds.max.x, bounds.min.y, bounds.min.z),
                        glm::vec3(bounds.max.x, bounds.min.y, bounds.max.z),
                        glm::vec3(bounds.max.x, bounds.max.y, bounds.min.z),
                        bounds.max
                };

                glm::vec3 worldMin(FLT_MAX), worldMax(-FLT_MAX);

                for (const glm::vec3& corner : corners)
                {
                    glm::vec3 worldCorner = glm::vec3(localToWorld * glm::vec4(corner, 1.0f));
                    worldMin = glm::min(worldMin, worldCorner);
                    worldMax = glm::max(worldMax, worldCorner);
                }

                auto& nodes = GetGraph()->GetAllNodes();
                std::vector<int> nodesToRemove;

                for (const auto& pair : nodes)
                {
                    int id = pair.first;
                    const Node& node = pair.second;
                    glm::vec2 pos = node.GetPosition();

                    if (pos.x >= worldMin.x && pos.x <= worldMax.x &&
                        pos.y >= worldMin.z && pos.y <= worldMax.z)
                    {
                        nodesToRemove.push_back(id);
                    }
                }

                for (int id : nodesToRemove)
                {
                    GetGraph()->RemoveNode(id);
                }
            }
        }
    }

    void NavMesh::ClearGraph()
    {
        NavGraph = nullptr;
    }

    void NavMesh::BakeNavMesh(Entity* Root)
    {
        if (!GetGraph())
        {
            ClearGraph();
        }
        BuildNavMesh(Root, Spacing);
        RemoveNotWalkableNodes(Root);

        if (GetGraph()->GetAllNodes().size() == 0)
        {
            spdlog::warn("NavGraph has 0 nodes!");
        }
    }

    void NavMesh::BuildNavMesh(Entity* Root, float Spacing)
    {
        std::vector<Transform*> entities = Root->GetTransform()->GetChildren();
        if (entities.empty())
            return;

        std::vector<Models::Model*> models;

        glm::vec2 sceneMin(FLT_MAX, FLT_MAX);
        glm::vec2 sceneMax(-FLT_MAX, -FLT_MAX);

        float largestModelSize = 1.0f;

        for (const auto& entity : entities)
        {
            auto* navArea = entity->GetOwner()->GetComponent<NavArea>();
            if (!navArea || !navArea->GetWalkable() || !entity->GetOwner()->GetComponent<ModelRenderer>())
                continue;

            Models::Model* model = entity->GetOwner()->GetComponent<ModelRenderer>()->GetModel();
            models.push_back(model);
            for (int i = 0; i < model->GetMeshCount(); ++i)
            {
                const Models::AABBox3& bounds = model->GetMesh(i)->GetAabBox();
                glm::mat4 localToWorld = entity->GetLocalToWorldMatrix();

                glm::vec3 corners[8] = {
                        bounds.min,
                        glm::vec3(bounds.min.x, bounds.min.y, bounds.max.z),
                        glm::vec3(bounds.min.x, bounds.max.y, bounds.min.z),
                        glm::vec3(bounds.min.x, bounds.max.y, bounds.max.z),
                        glm::vec3(bounds.max.x, bounds.min.y, bounds.min.z),
                        glm::vec3(bounds.max.x, bounds.min.y, bounds.max.z),
                        glm::vec3(bounds.max.x, bounds.max.y, bounds.min.z),
                        bounds.max
                };

                for (const glm::vec3& corner : corners)
                {
                    glm::vec3 worldCorner = glm::vec3(localToWorld * glm::vec4(corner, 1.0f));
                    sceneMin.x = std::min(sceneMin.x, worldCorner.x);
                    sceneMin.y = std::min(sceneMin.y, worldCorner.z);

                    sceneMax.x = std::max(sceneMax.x, worldCorner.x);
                    sceneMax.y = std::max(sceneMax.y, worldCorner.z);
                }

                float sizeX = bounds.max.x - bounds.min.x;
                float sizeZ = bounds.max.z - bounds.min.z;
                largestModelSize = std::max(largestModelSize, std::max(sizeX, sizeZ));
            }
        }

        int width = static_cast<int>((sceneMax.x - sceneMin.x) / Spacing) + 1;
        int depth = static_cast<int>((sceneMax.y - sceneMin.y) / Spacing) + 1;

        auto nodeId = [width](int X, int Y) { return Y * width + X; };

        for (int z = 0; z < depth; ++z)
        {
            for (int x = 0; x < width; ++x)
            {
                glm::vec3 position = {
                        sceneMin.x + (float) x * Spacing,
                        1000.0f,
                        sceneMin.y + (float) z * Spacing
                };

                bool foundSurface = false;
                glm::vec3 rayDir = glm::vec3(0.0f, -1.0f, 0.0f);

                for (const auto& model : models)
                {
                    for (int i = 0; i < model->GetMeshCount(); i++)
                    {
                        const auto& mesh = *model->GetMesh(i);
                        const auto& vertices = mesh.VerticesData;
                        const auto& indices = mesh.VertexIndices;

                        for (size_t j = 0; j + 2 < indices.size(); j += 3)
                        {
                            glm::vec3 v0 = vertices[indices[j + 0]].Position;
                            glm::vec3 v1 = vertices[indices[j + 1]].Position;
                            glm::vec3 v2 = vertices[indices[j + 2]].Position;

                            glm::vec3 hitPoint;
                            if (RayIntersectsTriangle(position, rayDir, v0, v1, v2, &hitPoint))
                            {
                                foundSurface = true;
                                break;
                            }
                        }

                        if (foundSurface)
                            break;
                    }

                    if (foundSurface)
                        break;
                }

                if (foundSurface)
                {
                    int id = nodeId(x, z);
                    GetGraph()->AddNode(id, glm::vec2(position.x, position.z));

                    if (x > 0)
                        GetGraph()->AddConnection(id, nodeId(x - 1, z));
                    if (z > 0)
                        GetGraph()->AddConnection(id, nodeId(x, z - 1));
                    if (x > 0 && z > 0)
                        GetGraph()->AddConnection(id, nodeId(x - 1, z - 1));
                    if (x < width - 1 && z > 0)
                        GetGraph()->AddConnection(id, nodeId(x + 1, z - 1));
                }
            }
        }
    }

    bool NavMesh::RayIntersectsTriangle(const glm::vec3& RayOrigin, const glm::vec3& RayDirection,
                                        const glm::vec3& Vertex0, const glm::vec3& Vertex1, const glm::vec3& Vertex2,
                                        glm::vec3* HitPoint)
    {
        const float epsilon = 1e-6f;

        glm::vec3 edge1 = Vertex1 - Vertex0;
        glm::vec3 edge2 = Vertex2 - Vertex0;

        glm::vec3 h = glm::cross(RayDirection, edge2);
        float determinant = glm::dot(edge1, h);

        if (determinant > -epsilon && determinant < epsilon)
            return false;

        float invDeterminant = 1.0f / determinant;
        glm::vec3 s = RayOrigin - Vertex0;
        float u = invDeterminant * glm::dot(s, h);

        if (u < 0.0f || u > 1.0f)
            return false;

        glm::vec3 q = glm::cross(s, edge1);
        float v = invDeterminant * glm::dot(RayDirection, q);

        if (v < 0.0f || u + v > 1.0f)
            return false;

        float t = invDeterminant * glm::dot(edge2, q);
        if (t > epsilon)
        {
            if (HitPoint)
                *HitPoint = RayOrigin + RayDirection * t;
            return true;
        }

        return false;
    }
}
