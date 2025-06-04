#include "NavMesh.h"
#include "Engine/Components/Renderers/ModelRenderer.h"
#include "spdlog/spdlog.h"
#include "Engine/EngineObjects/RayCast.h"

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

    bool NavMesh::IsOnNavMesh(const glm::vec3& Position, float MaxDistance)
    {
        const auto* graph = Engine::NavMesh::Get().GetGraph();
        if (!graph)
            return false;

        const auto& nodes = graph->GetAllNodes();
        for (const auto& [id, node] : nodes)
        {
            if (glm::distance(Position, node.GetPosition()) <= MaxDistance)
                return true;
        }

        return false;
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
                    glm::vec2 pos = glm::vec2(node.GetPosition().x, node.GetPosition().z);

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
        if (GetGraph())
        {
            ClearGraph();
            NavGraph = std::make_unique<Graph>();
        }
        BuildNavMesh(Root, Spacing);
        RemoveNotWalkableNodes(Root);
        for (int i = 0; i < Padding; i++)
            RemovePaddingNodes();

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

        std::vector<std::pair<Models::Model*, glm::mat4>> modelTransforms;

        glm::vec2 sceneMin(FLT_MAX, FLT_MAX);
        glm::vec2 sceneMax(-FLT_MAX, -FLT_MAX);

        float largestModelSize = 1.0f;

        for (const auto& entity : entities)
        {
            auto* navArea = entity->GetOwner()->GetComponent<NavArea>();
            if (!navArea || !navArea->GetWalkable() || !entity->GetOwner()->GetComponent<ModelRenderer>())
                continue;

            Models::Model* model = entity->GetOwner()->GetComponent<ModelRenderer>()->GetModel();
            glm::mat4 localToWorld = entity->GetLocalToWorldMatrix();
            modelTransforms.emplace_back(model, localToWorld);

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
        glm::vec3 finalHitPoint;

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
                float closestDist = std::numeric_limits<float>::max();
                glm::vec3 closestHit;
                bool hitSomething = false;

                for (const auto& modelPair : modelTransforms)
                {
                    Models::Model* model = modelPair.first;
                    const glm::mat4& localToWorld = modelPair.second;

                    for (int i = 0; i < model->GetMeshCount(); i++)
                    {
                        const auto& mesh = *model->GetMesh(i);
                        const auto& vertices = mesh.VerticesData;
                        const auto& indices = mesh.VertexIndices;

                        for (size_t j = 0; j + 2 < indices.size(); j += 3)
                        {
                            glm::vec3 v0 = glm::vec3(localToWorld * glm::vec4(vertices[indices[j + 0]].Position, 1.0f));
                            glm::vec3 v1 = glm::vec3(localToWorld * glm::vec4(vertices[indices[j + 1]].Position, 1.0f));
                            glm::vec3 v2 = glm::vec3(localToWorld * glm::vec4(vertices[indices[j + 2]].Position, 1.0f));

                            glm::vec3 hitPoint;
                            if (RayCast::RayIntersectsTriangle(position, rayDir, v0, v1, v2, &hitPoint))
                            {
                                float dist = glm::length(hitPoint - position);
                                if (dist < closestDist)
                                {
                                    closestDist = dist;
                                    closestHit = hitPoint;
                                    hitSomething = true;
                                }
                            }
                        }

                        if (hitSomething)
                        {
                            foundSurface = true;
                            finalHitPoint = closestHit;
                        }
                    }

                    if (foundSurface)
                        break;
                }

                if (foundSurface)
                {
                    int id = nodeId(x, z);
                    GetGraph()->AddNode(id, finalHitPoint);

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

    void NavMesh::RemovePaddingNodes()
    {
        auto& nodes = GetGraph()->GetAllNodes();
        std::vector<int> nodesToRemove;

        for (const auto& [id, node] : nodes)
        {
            const auto& neighbors = GetGraph()->GetNode(id).GetNeighbors();
            if (neighbors.size() < 8)
            {
                nodesToRemove.push_back(id);
            }
        }

        for (int id : nodesToRemove)
        {
            GetGraph()->RemoveNode(id);
        }
    }

    int NavMesh::GetNodeIdFromPosition(const glm::vec3& Position) const
    {
        if (!NavGraph)
            return -1;

        const auto& nodes = NavGraph->GetAllNodes();
        float closestDistance = FLT_MAX;
        int closestNodeId = -1;

        for (const auto& nodePair : nodes)
        {
            const glm::vec3& nodePosition = nodePair.second.GetPosition();

            float distance = glm::length(glm::vec2(Position.x, Position.z) - glm::vec2(nodePosition.x, nodePosition.z));

            if (distance < closestDistance)
            {
                closestDistance = distance;
                closestNodeId = nodePair.first;
            }
        }

        if (closestDistance > 2 * NavMesh::Get().GetSpacing())
        {
            return -1;
        }

        return closestNodeId;
    }
}
